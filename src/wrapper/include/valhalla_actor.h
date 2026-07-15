#ifndef VALHALLAACTOR_H
#define VALHALLAACTOR_H

#include <string>
#include <valhalla/tyr/actor.h>
#include <valhalla/baldr/tilegetter.h>

#ifdef __ANDROID__
#include <valhalla/baldr/curl_tilegetter.h>
#endif

class ValhallaMobileHttpClient {
public:
    virtual ~ValhallaMobileHttpClient() = default;
    
    /**
     * Makes a synchronous GET request to fetch tile data
     * @param url the URL to fetch
     * @param range_offset optional offset for range requests
     * @param range_size optional size for range requests
     * @return GET_response_t with the response data and status
     */
    virtual valhalla::baldr::tile_getter_t::GET_response_t 
    get(const std::string& url, uint64_t range_offset = 0, uint64_t range_size = 0) = 0;
    
    /**
     * Makes a synchronous HEAD request to fetch response headers
     * @param url the URL to query
     * @param header_mask mask for which headers to retrieve
     * @return HEAD_response_t with the response headers and status
     */
    virtual valhalla::baldr::tile_getter_t::HEAD_response_t
    head(const std::string& url, valhalla::baldr::tile_getter_t::header_mask_t header_mask) = 0;
};

#ifdef __ANDROID__
/**
 * Android implementation of ValhallaMobileHttpClient using libcurl (via the existing
 * curl_tile_getter_t), mirroring how the iOS wrapper implements the same interface with
 * NSURLSession in ValhallaWrapper.mm.
 */
class ValhallaMobileHttpClientImpl : public ValhallaMobileHttpClient {
public:
  // TODO: gzip is hardcoded off to match ValhallaDemo's current default; wire this up to
  // mjolnir.tile_url_gz if/when a caller needs gzip-compressed tiles.
  ValhallaMobileHttpClientImpl()
      : curl_getter_(/*pool_size=*/1, /*user_agent=*/"valhalla-mobile", /*gzipped=*/false,
                     /*user_pw=*/"") {
  }

  valhalla::baldr::tile_getter_t::GET_response_t
  get(const std::string& url, uint64_t range_offset = 0, uint64_t range_size = 0) override {
    return curl_getter_.get(url, range_offset, range_size);
  }

  valhalla::baldr::tile_getter_t::HEAD_response_t
  head(const std::string& url, valhalla::baldr::tile_getter_t::header_mask_t header_mask) override {
    return curl_getter_.head(url, header_mask);
  }

private:
  valhalla::baldr::curl_tile_getter_t curl_getter_;
};
#endif

class ValhallaActor {
private:
    std::unique_ptr<valhalla::tyr::actor_t> actor;
    std::unique_ptr<valhalla::baldr::GraphReader> graph_reader;
public:
    ValhallaActor(const std::string& config_path, ValhallaMobileHttpClient* http_client = nullptr);
    
    std::string route(const std::string& request);
};

#endif // VALHALLAACTOR_H
