////////////////////////////////////////////////////////////////////////////////
/// @brief abstract client connection
///
/// @file
///
/// DISCLAIMER
///
/// Copyright 2014 ArangoDB GmbH, Cologne, Germany
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
/// @author Copyright 2014, ArangoDB GmbH, Cologne, Germany
/// @author Copyright 2009-2013, triAGENS GmbH, Cologne, Germany
////////////////////////////////////////////////////////////////////////////////

#ifndef ARANGODB_SIMPLE_HTTP_CLIENT_GENERAL_CLIENT_CONNECTION_H
#define ARANGODB_SIMPLE_HTTP_CLIENT_GENERAL_CLIENT_CONNECTION_H 1

#include "Basics/Common.h"

#include "Basics/StringBuffer.h"
#include "Rest/Endpoint.h"

// -----------------------------------------------------------------------------
// --SECTION--                                           GeneralClientConnection
// -----------------------------------------------------------------------------

namespace triagens {
  namespace httpclient {

////////////////////////////////////////////////////////////////////////////////
/// @brief general client connection
////////////////////////////////////////////////////////////////////////////////

    class GeneralClientConnection {

// -----------------------------------------------------------------------------
// --SECTION--                                                          typedefs
// -----------------------------------------------------------------------------

      protected:

////////////////////////////////////////////////////////////////////////////////
/// @brief size of read buffer for read operations
////////////////////////////////////////////////////////////////////////////////

        enum { READBUFFER_SIZE = 8192 };

// -----------------------------------------------------------------------------
// --SECTION--                                        constructors / destructors
// -----------------------------------------------------------------------------

      private:

        GeneralClientConnection (GeneralClientConnection const&);
        GeneralClientConnection& operator= (GeneralClientConnection const&);

      public:

////////////////////////////////////////////////////////////////////////////////
/// @brief creates a new client connection
////////////////////////////////////////////////////////////////////////////////

        GeneralClientConnection (triagens::rest::Endpoint* endpoint,
                                 double,
                                 double,
                                 size_t);

////////////////////////////////////////////////////////////////////////////////
/// @brief destroys a client connection
////////////////////////////////////////////////////////////////////////////////

        virtual ~GeneralClientConnection ();

// -----------------------------------------------------------------------------
// --SECTION--                                                    public methods
// -----------------------------------------------------------------------------

      public:

////////////////////////////////////////////////////////////////////////////////
/// @brief create a new connection from an endpoint
////////////////////////////////////////////////////////////////////////////////

        static GeneralClientConnection* factory (triagens::rest::Endpoint*,
                                                 double,
                                                 double,
                                                 size_t,
                                                 uint32_t);

////////////////////////////////////////////////////////////////////////////////
/// @brief return the endpoint
////////////////////////////////////////////////////////////////////////////////

        triagens::rest::Endpoint const* getEndpoint () const {
          return _endpoint;
        }

////////////////////////////////////////////////////////////////////////////////
/// @brief returns a string representation of the connection endpoint
////////////////////////////////////////////////////////////////////////////////

        std::string getEndpointSpecification () const {
          return _endpoint->getSpecification();
        }

////////////////////////////////////////////////////////////////////////////////
/// @brief return the connection state
////////////////////////////////////////////////////////////////////////////////

        bool isConnected () const {
          return _isConnected;
        }

////////////////////////////////////////////////////////////////////////////////
/// @brief reset the number of connect attempts
////////////////////////////////////////////////////////////////////////////////

        void resetNumConnectRetries () {
          _numConnectRetries = 0;
        }

////////////////////////////////////////////////////////////////////////////////
/// @brief returns the number of configured connect retries
////////////////////////////////////////////////////////////////////////////////
        
        inline size_t connectRetries () const {
           return _connectRetries;
        }

////////////////////////////////////////////////////////////////////////////////
/// @brief connect
////////////////////////////////////////////////////////////////////////////////

        bool connect ();

////////////////////////////////////////////////////////////////////////////////
/// @brief disconnect
////////////////////////////////////////////////////////////////////////////////

        void disconnect ();

////////////////////////////////////////////////////////////////////////////////
/// @brief send data to the endpoint
////////////////////////////////////////////////////////////////////////////////

        bool handleWrite (const double, void*, size_t, size_t*);

////////////////////////////////////////////////////////////////////////////////
/// @brief read data from endpoint
////////////////////////////////////////////////////////////////////////////////

        bool handleRead (double, triagens::basics::StringBuffer&, 
                         bool& connectionClosed);

////////////////////////////////////////////////////////////////////////////////
/// @brief return the endpoint
////////////////////////////////////////////////////////////////////////////////

        const std::string& getErrorDetails () const {
          return _errorDetails;
        }

// -----------------------------------------------------------------------------
// --SECTION--                                         protected virtual methods
// -----------------------------------------------------------------------------

      protected:

////////////////////////////////////////////////////////////////////////////////
/// @brief connect
////////////////////////////////////////////////////////////////////////////////

        virtual bool connectSocket () = 0;

////////////////////////////////////////////////////////////////////////////////
/// @brief disconnect
////////////////////////////////////////////////////////////////////////////////

        virtual void disconnectSocket () = 0;

////////////////////////////////////////////////////////////////////////////////
/// @brief prepare connection for read/write I/O
////////////////////////////////////////////////////////////////////////////////

        virtual bool prepare (double, bool) const = 0;

////////////////////////////////////////////////////////////////////////////////
/// @brief write data to the connection
////////////////////////////////////////////////////////////////////////////////

        virtual bool writeClientConnection (void*, size_t, size_t*) = 0;

////////////////////////////////////////////////////////////////////////////////
/// @brief read data from the connection
////////////////////////////////////////////////////////////////////////////////

        virtual bool readClientConnection (triagens::basics::StringBuffer&, bool& porgress) = 0;

////////////////////////////////////////////////////////////////////////////////
/// @brief return whether the connection is readable
////////////////////////////////////////////////////////////////////////////////

        virtual bool readable () = 0;

// -----------------------------------------------------------------------------
// --SECTION--                                               protected variables
// -----------------------------------------------------------------------------

      protected:

////////////////////////////////////////////////////////////////////////////////
/// @brief details to errors
////////////////////////////////////////////////////////////////////////////////

        mutable std::string _errorDetails;

////////////////////////////////////////////////////////////////////////////////
/// @brief endpoint to connect to
////////////////////////////////////////////////////////////////////////////////

        triagens::rest::Endpoint* _endpoint;

////////////////////////////////////////////////////////////////////////////////
/// @brief request timeout (in seconds)
////////////////////////////////////////////////////////////////////////////////

        double _requestTimeout;

////////////////////////////////////////////////////////////////////////////////
/// @brief connection timeout (in seconds)
////////////////////////////////////////////////////////////////////////////////

        double _connectTimeout;

////////////////////////////////////////////////////////////////////////////////
/// @brief allowed number of connect retries
////////////////////////////////////////////////////////////////////////////////

        size_t _connectRetries;

////////////////////////////////////////////////////////////////////////////////
/// @brief number of connection attempts made
////////////////////////////////////////////////////////////////////////////////

        size_t _numConnectRetries;

////////////////////////////////////////////////////////////////////////////////
/// @brief whether we're connected
////////////////////////////////////////////////////////////////////////////////

        bool _isConnected;

    };
  }
}

#endif
// -----------------------------------------------------------------------------
// --SECTION--                                                       END-OF-FILE
// -----------------------------------------------------------------------------

// Local Variables:
// mode: outline-minor
// outline-regexp: "/// @brief\\|/// {@inheritDoc}\\|/// @page\\|// --SECTION--\\|/// @\\}"
// End:
