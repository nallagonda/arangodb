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

#ifndef ARANGODB_IN_MESSAGE_CACHE_H
#define ARANGODB_IN_MESSAGE_CACHE_H 1

#include <string>
#include <velocypack/vpack.h>
#include <velocypack/velocypack-aliases.h>

#include "Basics/Common.h"
#include "Basics/Mutex.h"

#include "Message.h"

namespace arangodb {
  //class Mutex;
  
namespace pregel {
    class WorkerContext;
    
    //template <typename T>
    class MessageIterator {
        friend class InMessageCache;
    public:
        MessageIterator() : _size(0) {}
        
        typedef MessageIterator iterator;
        typedef const MessageIterator const_iterator;
        
        explicit MessageIterator(VPackSlice slice) : _slice(slice) {
            if (_slice.isNull() || _slice.isNone()) _size = 0;
            else if (_slice.isArray()) _size = _slice.length();
            else _size = 1;
        }
        
        iterator begin() {
            return MessageIterator(_slice);
        }
        const_iterator begin() const {
            return MessageIterator(_slice);
        }
        iterator end() {
            auto it = MessageIterator(_slice);
            it._position = it._size;
            return it;
        }
        const_iterator end() const {
            auto it = MessageIterator(_slice);
            it._position = it._size;
            return it;
        }
        Message operator*() const {
            if (_slice.isArray()) {
                return Message(_slice.at(_position));
            } else {
                return Message(_slice);
            }
        }
        
        // prefix ++
        MessageIterator& operator++() {
            ++_position;
            return *this;
        }
        
        // postfix ++
        MessageIterator operator++(int) {
            MessageIterator result(*this);
            ++(*this);
            return result;
        }
        
        bool operator!=(MessageIterator const& other) const {
            return _position != other._position;
        }
        
        size_t size() const {
            return _size;
        }
        
    private:
        VPackSlice _slice;
        size_t _position = 0;
        size_t _size = 1;
    };
    
/* In the longer run, maybe write optimized implementations for certain use cases. For example threaded
 processing */
class InMessageCache {
    friend class OutMessageCache;
public:
    InMessageCache(){}//std::shared_ptr<WorkerContext> context
  ~InMessageCache();
  
  void parseMessages(VPackSlice messages);
  /// @brief get messages for vertex id. (Don't use keys from _from or _to directly, they contain the collection name)
  MessageIterator getMessages(std::string const& vertexId);
  void clear();
  
protected:
    /// @brief internal method to direclty set the messages for a vertex. Only valid with already combined messages
    void setDirect(std::string const& vertexId, VPackSlice data);
private:
  std::unordered_map<std::string, VPackBuilder*> _messages;
  Mutex writeMutex;
  //std::shared_ptr<WorkerContext> _ctx;
};

}}
#endif
