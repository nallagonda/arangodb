////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2014-2018 ArangoDB GmbH, Cologne, Germany
/// Copyright 2004-2014 triAGENS GmbH, Cologne, Germany
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///     http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
/// Copyright holder is ArangoDB GmbH, Cologne, Germany
///
/// @author Kaveh Vahedipour
/// @author Matthew Von-Maszewski
////////////////////////////////////////////////////////////////////////////////

#include "CreateCollection.h"
#include "MaintenanceFeature.h"

#include "ApplicationFeatures/ApplicationServer.h"
#include "Basics/VelocyPackHelper.h"
#include "Cluster/ClusterFeature.h"
#include "Cluster/FollowerInfo.h"
#include "VocBase/LogicalCollection.h"
#include "VocBase/Methods/Collections.h"
#include "VocBase/Methods/Databases.h"

#include <velocypack/Compare.h>
#include <velocypack/Iterator.h>
#include <velocypack/Slice.h>
#include <velocypack/velocypack-aliases.h>


using namespace arangodb::application_features;
using namespace arangodb::maintenance;
using namespace arangodb::methods;

constexpr auto WAIT_FOR_SYNC_REPL = "waitForSyncReplication";
constexpr auto ENF_REPL_FACT = "enforceReplicationFactor";

CreateCollection::CreateCollection(
  MaintenanceFeature& feature, ActionDescription const& desc)
  : ActionBase(feature, desc) {

  if (!desc.has(DATABASE)) {
    LOG_TOPIC(ERR, Logger::MAINTENANCE)
      << "CreateCollection: database must be specified";
    setState(FAILED);
  }
  TRI_ASSERT(desc.has(DATABASE));
  
  if (!desc.has(COLLECTION)) {
    LOG_TOPIC(ERR, Logger::MAINTENANCE)
      << "CreateCollection: cluster-wide collection must be specified";
    setState(FAILED);
  }
  TRI_ASSERT(desc.has(COLLECTION));
  
  if (!desc.has(SHARD)) {
    LOG_TOPIC(ERR, Logger::MAINTENANCE)
      << "CreateCollection: shard must be specified";
    setState(FAILED);
  }
  TRI_ASSERT(desc.has(SHARD));
  
  if (!desc.has(LEADER)) {
    LOG_TOPIC(ERR, Logger::MAINTENANCE)
      << "CreateCollection: shard leader must be specified";
    setState(FAILED);
  }
  TRI_ASSERT(desc.has(LEADER));

  if (!properties().hasKey(TYPE) || !properties().get(TYPE).isNumber()) {
    LOG_TOPIC(ERR, Logger::MAINTENANCE)
      << "CreateCollection: properties slice must specify collection type";
    setState(FAILED);
  }
  TRI_ASSERT(properties().hasKey(TYPE) && properties().get(TYPE).isNumber());

  uint32_t const type = properties().get(TYPE).getNumber<uint32_t>();
  if (type != TRI_COL_TYPE_DOCUMENT && type != TRI_COL_TYPE_EDGE) {
    LOG_TOPIC(ERR, Logger::MAINTENANCE)
      << "CreateCollection: invalid collection type number " << type;
    setState(FAILED);    
  }
  TRI_ASSERT(type == TRI_COL_TYPE_DOCUMENT || type == TRI_COL_TYPE_EDGE);
  
}

CreateCollection::~CreateCollection() {};



bool CreateCollection::first() {

  auto const& database = _description.get(DATABASE);
  auto const& collection = _description.get(COLLECTION);
  auto const& shard = _description.get(SHARD);
  auto const& leader = _description.get(LEADER);
  auto const& props = properties();

  LOG_TOPIC(DEBUG, Logger::MAINTENANCE)
    << "creating local shard '" << database << "/" << shard
    << "' for central '" << database << "/" << collection << "'";

  auto vocbase = Databases::lookup(database);
  if (vocbase == nullptr) {
    std::string errorMsg("CreateCollection: Failed to lookup database ");
    errorMsg += database;
    _result.reset(TRI_ERROR_ARANGO_DATABASE_NOT_FOUND, errorMsg);
    setState(FAILED);
    return false;
  }

  auto cluster =
    ApplicationServer::getFeature<ClusterFeature>("Cluster");

  bool waitForRepl =
    (props.hasKey(WAIT_FOR_SYNC_REPL) &&
     props.get(WAIT_FOR_SYNC_REPL).isBool()) ?
    props.get(WAIT_FOR_SYNC_REPL).getBool() :
    cluster->createWaitsForSyncReplication();

  bool enforceReplFact =
    (props.hasKey(ENF_REPL_FACT) &&
     props.get(ENF_REPL_FACT).isBool()) ?
    props.get(ENF_REPL_FACT).getBool() : true;

  TRI_col_type_e type(props.get(TYPE).getNumber<TRI_col_type_e>());

  VPackBuilder docket;
  { VPackObjectBuilder d(&docket);
    for (auto const& i : VPackObjectIterator(props)) {
      auto const& key = i.key.copyString();
      if (key == ID || key == NAME || key == GLOB_UID || key == OBJECT_ID) {
        if (key == GLOB_UID || key == OBJECT_ID) {
          LOG_TOPIC(WARN, Logger::MAINTENANCE)
            << "unexpected " << key << " in " << props.toJson();
        }
        continue;
      }
      docket.add(key, i.value);
    }
    docket.add("planId", VPackValue(collection));
  }

  _result = Collections::create(
    vocbase, shard, type, docket.slice(), waitForRepl, enforceReplFact,
    [=](LogicalCollection& col) {
      LOG_TOPIC(DEBUG, Logger::MAINTENANCE) << "local collection " << database
      << "/" << shard << " successfully created";
      col.followers()->setTheLeader(leader);
      if (leader.empty()) {
        col.followers()->clear();
      }
    });
  
  if (_result.fail()) {
    LOG_TOPIC(ERR, Logger::MAINTENANCE)
      << "creating local shard '" << database << "/" << shard
      << "' for central '" << database << "/" << collection << "' failed: "
      << _result;
    setState(FAILED);
    return false;
  }

  setState(COMPLETE);
  return false;

}
