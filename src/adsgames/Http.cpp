#include "Http.h"

#include <asw/asw.h>
#include <ixwebsocket/IXHttpClient.h>
#include <ixwebsocket/IXNetSystem.h>

namespace {
constexpr int CONNECT_TIMEOUT_S = 5;
constexpr int TRANSFER_TIMEOUT_S = 600;
constexpr int HTTP_OK = 200;
}  // namespace

std::optional<std::string> adsgames::http::get(const std::string& url,
                                               const Progress& progress) {
  // Needed once on Windows, does nothing elsewhere
  static const bool net_ready = ix::initNetSystem();
  (void)net_ready;

  ix::HttpClient client;
  const auto args = client.createRequest(url, ix::HttpClient::kGet);
  args->connectTimeout = CONNECT_TIMEOUT_S;
  args->transferTimeout = TRANSFER_TIMEOUT_S;

  // Built without zlib
  args->compress = false;

  if (progress) {
    args->onProgressCallback = [&progress](int received, int total) {
      return progress(static_cast<uint64_t>(received),
                      static_cast<uint64_t>(total));
    };
  }

  const auto response = client.get(url, args);

  if (response->errorCode != ix::HttpErrorCode::Ok) {
    asw::log::warn("GET {} failed: {}", url, response->errorMsg);
    return std::nullopt;
  }

  if (response->statusCode != HTTP_OK) {
    asw::log::warn("GET {} returned {}", url, response->statusCode);
    return std::nullopt;
  }

  return response->body;
}
