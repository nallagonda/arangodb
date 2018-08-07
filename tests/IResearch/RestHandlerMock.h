////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2018 ArangoDB GmbH, Cologne, Germany
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
/// @author Andrey Abramov
/// @author Vasiliy Nabatchikov
////////////////////////////////////////////////////////////////////////////////

#ifndef ARANGODB_IRESEARCH__REST_HANDLER_MOCK_H
#define ARANGODB_IRESEARCH__REST_HANDLER_MOCK_H 1

#include "Rest/GeneralRequest.h"
#include "Rest/GeneralResponse.h"

class TRI_vocbase_t; // forward declaration

namespace arangodb {

class VocbaseContext; // forward declaration

}

struct GeneralRequestMock: public arangodb::GeneralRequest {
  std::unordered_map<std::string, std::vector<std::string>> _arrayValues;
  int64_t _contentLength;
  std::unique_ptr<arangodb::VocbaseContext> _context; // VocbaseContext required for use with RestVocbaseBaseHandler
  std::unordered_map<std::string, std::string> _headers; // request headers parameters
  arangodb::velocypack::Builder _payload; // request body
  std::unordered_map<std::string, std::string> _values; // query string parameters

  GeneralRequestMock(TRI_vocbase_t& vocbase);
  virtual std::unordered_map<std::string, std::vector<std::string>> arrayValues() const override;
  virtual int64_t contentLength() const override;
  virtual std::string const& header(std::string const& key) const override;
  virtual std::string const& header(std::string const& key, bool& found) const override;
  virtual std::unordered_map<std::string, std::string> const& headers() const override;
  virtual arangodb::velocypack::Slice payload(arangodb::velocypack::Options const* options = &arangodb::velocypack::Options::Defaults) override;
  virtual arangodb::Endpoint::TransportType transportType() override;
  virtual std::string const& value(std::string const& key) const override;
  virtual std::string const& value(std::string const& key, bool& found) const override;
  virtual std::unordered_map<std::string, std::string> const& values() const override;
};

struct GeneralResponseMock: public arangodb::GeneralResponse {
  arangodb::velocypack::Builder _payload;

  GeneralResponseMock(arangodb::ResponseCode code = arangodb::ResponseCode::OK);
  virtual void addPayload(arangodb::velocypack::Buffer<uint8_t>&& buffer, arangodb::velocypack::Options const* options = nullptr, bool resolveExternals = true) override;
  virtual void addPayload(arangodb::velocypack::Slice const& slice, arangodb::velocypack::Options const* options = nullptr, bool resolveExternals = true) override;
  virtual void reset(arangodb::ResponseCode code) override;
  virtual arangodb::Endpoint::TransportType transportType() override;
};

#endif