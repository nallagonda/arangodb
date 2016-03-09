////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2014-2016 ArangoDB GmbH, Cologne, Germany
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
/// @author Jan Steemann
////////////////////////////////////////////////////////////////////////////////

#ifndef ARANGOD_UTILS_SINGLE_COLLECTION_TRANSACTION_H
#define ARANGOD_UTILS_SINGLE_COLLECTION_TRANSACTION_H 1

#include "Basics/Common.h"
#include "Utils/Transaction.h"
#include "VocBase/transaction.h"
#include "VocBase/voc-types.h"

namespace arangodb {
class DocumentDitch;
class TransactionContext;

class SingleCollectionTransaction : public Transaction {

 public:
  //////////////////////////////////////////////////////////////////////////////
  /// @brief create the transaction, using a collection id
  //////////////////////////////////////////////////////////////////////////////

  SingleCollectionTransaction(std::shared_ptr<TransactionContext>,
                              TRI_voc_cid_t, TRI_transaction_type_e);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief create the transaction, using a collection name
  //////////////////////////////////////////////////////////////////////////////

  SingleCollectionTransaction(std::shared_ptr<TransactionContext>,
                              std::string const&, TRI_transaction_type_e);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief end the transaction
  //////////////////////////////////////////////////////////////////////////////

  ~SingleCollectionTransaction() = default;

 private:
  //////////////////////////////////////////////////////////////////////////////
  /// @brief get the underlying transaction collection
  //////////////////////////////////////////////////////////////////////////////

  TRI_transaction_collection_t* trxCollection();

 public:
  //////////////////////////////////////////////////////////////////////////////
  /// @brief get the underlying document collection
  /// note that we have two identical versions because this is called
  /// in two different situations
  //////////////////////////////////////////////////////////////////////////////

  TRI_document_collection_t* documentCollection();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief get the underlying collection's id
  //////////////////////////////////////////////////////////////////////////////
  
  inline TRI_voc_cid_t cid() const { return _cid; }

  //////////////////////////////////////////////////////////////////////////////
  /// @brief return the ditch for the collection
  /// note that the ditch must already exist
  /// furthermore note that we have two calling conventions because this
  /// is called in two different ways
  //////////////////////////////////////////////////////////////////////////////

  arangodb::DocumentDitch* ditch() const;

  arangodb::DocumentDitch* ditch(TRI_voc_cid_t) const;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief whether or not a ditch is available for a collection
  //////////////////////////////////////////////////////////////////////////////

  bool hasDitch() const;
  
  //////////////////////////////////////////////////////////////////////////////
  /// @brief get the underlying collection's name
  //////////////////////////////////////////////////////////////////////////////

  std::string name() const;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief explicitly lock the underlying collection for read access
  //////////////////////////////////////////////////////////////////////////////

  int lockRead();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief explicitly unlock the underlying collection after read access
  //////////////////////////////////////////////////////////////////////////////

  int unlockRead();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief explicitly lock the underlying collection for write access
  //////////////////////////////////////////////////////////////////////////////

  int lockWrite();
  
 private:
  //////////////////////////////////////////////////////////////////////////////
  /// @brief collection id
  //////////////////////////////////////////////////////////////////////////////

  TRI_voc_cid_t _cid;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief trxCollection cache
  //////////////////////////////////////////////////////////////////////////////

  TRI_transaction_collection_t* _trxCollection;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief TRI_document_collection_t* cache
  //////////////////////////////////////////////////////////////////////////////

  TRI_document_collection_t* _documentCollection;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief collection access type
  //////////////////////////////////////////////////////////////////////////////

  TRI_transaction_type_e _accessType;
};
}

#endif
