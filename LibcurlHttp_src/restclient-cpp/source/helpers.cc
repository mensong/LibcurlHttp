/**
 * @file helpers.cpp
 * @brief implementation of the restclient helpers
 * @author Daniel Schauenberg <d@unwiredcouch.com>
 */

#include "restclient-cpp/helpers.h"

#include <cstring>

#include "restclient-cpp/restclient.h"

/**
 * @brief write callback function for libcurl
 *
 * @param data returned data of size (size*nmemb)
 * @param size size parameter
 * @param nmemb memblock parameter
 * @param userdata pointer to user data to save/work with return data
 *
 * @return (size * nmemb)
 */
size_t RestClient::Helpers::write_callback(void *data, size_t size,
                                           size_t nmemb, void *userdata) {
  RestClient::Response* r;
  r = reinterpret_cast<RestClient::Response*>(userdata);
  r->body.append(reinterpret_cast<char*>(data), size*nmemb);

  return (size * nmemb);
}

/**
 * @brief header callback for libcurl
 *
 * @param data returned (header line)
 * @param size of data
 * @param nmemb memblock
 * @param userdata pointer to user data object to save headr data
 * @return size * nmemb;
 */
size_t RestClient::Helpers::header_callback(void *data, size_t size,
                                            size_t nmemb, void *userdata) {
  RestClient::Response* r;
  r = reinterpret_cast<RestClient::Response*>(userdata);
  std::string header(reinterpret_cast<char*>(data), size*nmemb);
  size_t seperator = header.find_first_of(':');
  if ( std::string::npos == seperator ) {
    // roll with non seperated headers...
    trim(header);
    if (0 == header.length()) {
      return (size * nmemb);  // blank line;
    }
	r->headers[header].clear();//Gergul
    r->headers[header].push_back("present");//Gergul
  } else {
    std::string key = header.substr(0, seperator);
    trim(key);
    std::string value = header.substr(seperator + 1);
    trim(value);
    r->headers[key].push_back(value);//Gergul
  }

  return (size * nmemb);
}

/**
 * @brief read callback function for libcurl
 *
 * @param data pointer of max size (size*nmemb) to write data to
 * @param size size parameter
 * @param nmemb memblock parameter
 * @param userdata pointer to user data to read data from
 *
 * @return (size * nmemb)
 */
size_t RestClient::Helpers::read_callback(void *data, size_t size,
                                          size_t nmemb, void *userdata) {
  /** get upload struct */
  RestClient::Helpers::UploadObject* u;
  u = reinterpret_cast<RestClient::Helpers::UploadObject*>(userdata);
  /** set correct sizes */
  size_t curl_size = size * nmemb;
  size_t copy_size = (u->length < curl_size) ? u->length : curl_size;
  /** copy data to buffer */
  std::memcpy(data, u->data, copy_size);
  /** decrement length and increment data pointer */
  u->length -= copy_size;
  u->data += copy_size;
  /** return copied size */
  return copy_size;
}

//
//// gzCompress: do the compressing  
//int gzCompress(const char *src, int srcLen, char *dest, int destLen)
//{
//	z_stream c_stream;
//	int err = 0;
//	int windowBits = 15;
//	int GZIP_ENCODING = 16;
//
//	if (src && srcLen > 0)
//	{
//		c_stream.zalloc = (alloc_func)0;
//		c_stream.zfree = (free_func)0;
//		c_stream.opaque = (voidpf)0;
//		if (deflateInit2(&c_stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED,
//			windowBits | GZIP_ENCODING, 8, Z_DEFAULT_STRATEGY) != Z_OK) return -1;
//		c_stream.next_in = (Bytef *)src;
//		c_stream.avail_in = srcLen;
//		c_stream.next_out = (Bytef *)dest;
//		c_stream.avail_out = destLen;
//		while (c_stream.avail_in != 0 && c_stream.total_out < destLen)
//		{
//			if (deflate(&c_stream, Z_NO_FLUSH) != Z_OK) return -1;
//		}
//		if (c_stream.avail_in != 0) return c_stream.avail_in;
//		for (;;) {
//			if ((err = deflate(&c_stream, Z_FINISH)) == Z_STREAM_END) break;
//			if (err != Z_OK) return -1;
//		}
//		if (deflateEnd(&c_stream) != Z_OK) return -1;
//		return c_stream.total_out;
//	}
//	return -1;
//}
//
//// gzDecompress: do the decompressing  
//int gzDecompress(const char *src, int srcLen, const char *dst, int dstLen) {
//	z_stream strm;
//	strm.zalloc = NULL;
//	strm.zfree = NULL;
//	strm.opaque = NULL;
//
//	strm.avail_in = srcLen;
//	strm.avail_out = dstLen;
//	strm.next_in = (Bytef *)src;
//	strm.next_out = (Bytef *)dst;
//
//	int err = -1, ret = -1;
//	err = inflateInit2(&strm, MAX_WBITS + 16);
//	if (err == Z_OK) {
//		err = inflate(&strm, Z_FINISH);
//		if (err == Z_STREAM_END) {
//			ret = strm.total_out;
//		}
//		else {
//			inflateEnd(&strm);
//			return err;
//		}
//	}
//	else {
//		inflateEnd(&strm);
//		return err;
//	}
//	inflateEnd(&strm);
//	return err;
//}
