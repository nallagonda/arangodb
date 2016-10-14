////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2016 ArangoDB GmbH, Cologne, Germany
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
/// @author Simon Grätzer
////////////////////////////////////////////////////////////////////////////////

#ifndef ARANGODB_PREGEL_UTILS_H
#define ARANGODB_PREGEL_UTILS_H 1

#include "Basics/Common.h"


struct TRI_vocbase_t;
namespace arangodb {
    
namespace pregel {
    
  class Utils {
    Utils() = delete;
    
  public:
    // constants
    static std::string const apiPrefix;
      
    static std::string const nextGSSPath;
    static std::string const finishedGSSPath;
    static std::string const messagesPath;
    static std::string const writeResultsPath;
    
    static std::string const executionNumberKey;
    static std::string const vertexCollectionNameKey;
    static std::string const vertexCollectionPlanIdKey;
    static std::string const vertexShardsListKey;
    static std::string const edgeShardsListKey;
    static std::string const resultShardKey;
    static std::string const algorithmKey;
    static std::string const coordinatorIdKey;
    
    static std::string const globalSuperstepKey;
    static std::string const messagesKey;
    static std::string const senderKey;
    static std::string const doneKey;
      
    static std::string const edgeShardingKey;
    static std::string baseUrl(std::string dbName);
    static std::string vertexKeyFromToValue(std::string const& graphKey);

  };
}
}
#endif
