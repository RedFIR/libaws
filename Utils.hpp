/*
 * Copyright (c) 2014 Smyle SAS
 * http: *www.deepomatic.com/
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#ifndef __LIB_AWS_UTILS__
#define __LIB_AWS_UTILS__

#include <iostream>
#include <ctime>
#include <sstream>
#include <array>
#include <cstring>
#include <stdexcept>

namespace LIBAWS {
	namespace Utils {
	// call the url and put the body in the stringstream
	void 		executeRequest(const std::string &url, std::stringstream &ss);

	// return the queueUrl from the stringstream
	std::string getQueueUrl(std::stringstream &ss);

	// return the canonicalUri from the stringstream
	std::string getQueueCanonicalUri(std::string &queueUrl);

	// overload of std::replace in order to replace one character by a string
	std::string replace(const std::string::iterator &begin, const std::string::iterator &end, const char from, const std::string &to);

	// return the current date formated as Format date as YYYYMMDD'T'HHMMSS'Z'
	std::string getAmzDate(std::time_t &t);
	//return the current date such as YYYYMMDD
	std::string getDatestamp(std::time_t &t);


		static void _sprintf(std::string &buffer, const char *str) {
			while (*str) {
				if (*str != '%'){
					buffer += *str;
				}
		        str++;
		    }
		}

		template <typename... Args>
		static void _sprintf(std::string & buffer, const char *str, const char * val, Args... args) {
			std::string tmp;

			while (*str) {
		       if (*str == '%') {
		            buffer += val;
		            str++;
		            _sprintf(buffer, str, (args)...);
		            return;
		        }
		        if (*str != '%'){
		        	buffer +=  *str;
		        }
		        str++;
		    }
		}

		template <typename... Args>
		static void _sprintf(std::string & buffer, const char *str, const std::string val, Args... args) {
			std::string tmp;
			
			while (*str) {
		       if (*str == '%') {
		            buffer += val;
		            str++;
		            _sprintf(buffer, str, (args)...);
		            return;
		        }
		        if (*str != '%'){
		        	buffer +=  *str;
		        }
		        str++;
		    }
		}

		template <typename T, typename... Args>
		static void _sprintf(std::string & buffer, const char *str, const T val, Args... args) {
			std::string tmp;
			std::stringstream ss; 

			while (*str) {
		       if (*str == '%') {
		            ss << val;
		            ss >> tmp;
		            buffer += tmp;
		            str++;
		            _sprintf(buffer, str, (args)...);
		            return;
		        }
		        if (*str != '%'){
		        	buffer +=  *str;
		        }
		        str++;
		    }
		}

		template<typename... Args>
		std::string sprintf(const std::string &s, Args... args) {
			std::string buffer;
			_sprintf(buffer, s.c_str(), args...);
			return std::move(buffer);
		}

	};
};


#endif