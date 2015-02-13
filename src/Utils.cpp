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

#include "Utils.hpp"
#include "Crypto.hpp"
#include "curlIncludes/curl_easy.h"
using curl::curl_easy;

using namespace LIBAWS;


std::string Utils::AWSAuth::generateUrl(const std::string &canonicalUri, const std::string &action, const std::string &params) const {
  std::time_t t = std::time(nullptr);
  std::string amzDate =  Utils::getAmzDate(t); 
  std::string datestamp = Utils::getDatestamp(t);

  std::string canonicalHeaders = "host:" + this->_host + "\n";
  std::string signedHeaders = "host";

  std::string algorithm = "AWS4-HMAC-SHA256";
  std::string credentialScope = datestamp + "/" + this->_region + "/" + this->_service + "/" + "aws4_request";

  std::string canonicalQuerystring = "Action=" + action + params; // TODO: verif queueName
  canonicalQuerystring += "&X-Amz-Algorithm=AWS4-HMAC-SHA256";
  std::string tmp = this->_awsKeyID + "/" + credentialScope;
  canonicalQuerystring += "&X-Amz-Credential=" + Utils::replace(tmp.begin(), tmp.end(), '/', "%2F"); //urllib.quote_plus
  canonicalQuerystring += "&X-Amz-Date=" + amzDate;
  canonicalQuerystring += "&X-Amz-Expires=30";
  canonicalQuerystring += "&X-Amz-SignedHeaders=" + signedHeaders;

  std::string payloadHash = Crypto::shaDigest();

  std::string canonicalRequest = this->_method + '\n' + canonicalUri + '\n' + canonicalQuerystring + '\n' + canonicalHeaders + '\n' + signedHeaders + '\n' + payloadHash;

  std::string stringToSign = algorithm + '\n' +  amzDate + '\n' +  credentialScope + '\n' +  Crypto::shaDigest(canonicalRequest);

  std::array<byte, 32> signingKey = Crypto::getSignatureKey(this->_awsSecretKey, datestamp, this->_region, this->_service);

  auto _sign = Crypto::sign(signingKey, stringToSign);
  std::string signature = Crypto::hexDump(_sign);
  std::transform(signature.begin(), signature.end(), signature.begin(), ::tolower);


  canonicalQuerystring += "&X-Amz-Signature=" + signature;
  std::string requestUrl = this->_endpoint + canonicalUri + "?" + canonicalQuerystring;

  #ifdef DEBUG
    std::cout << requestUrl << std::endl;
  #endif

  return std::move(requestUrl);
}

std::string Utils::replace(const std::string::iterator &begin, const std::string::iterator &end, const char from, const std::string &to){
	std::string rep("");

	for (auto it = begin; it != end;++it) {
		if (*it != '/') {
			rep += *it;
		}
		else {
			rep += to;
		}
	}
	return std::move(rep);
}

std::string Utils::getAmzDate(std::time_t &t) {
      std::tm* now_tm = std::gmtime(&t);
      char mbstr[100] = {0};

      if (std::strftime(mbstr, sizeof(mbstr), "%Y%m%dT%H%M%SZ", now_tm)) {
        return std::move(std::string(mbstr));
     }
     throw (std::runtime_error("strftime error"));
}

std::string Utils::getDatestamp(std::time_t &t) {
      std::tm* now_tm = std::gmtime(&t);
      char mbstr[100] = {0};

      if (std::strftime(mbstr, sizeof(mbstr), "%Y%m%d", now_tm)) {
        return std::move(std::string(mbstr));
     }
     throw (std::runtime_error("strftime error"));
}



#include <libxml++/libxml++.h>

void print_node(const xmlpp::Node* node, unsigned int indentation = 0)
{
  const Glib::ustring indent(indentation, ' ');
  std::cout << std::endl; //Separate nodes by an empty line.
  
  const xmlpp::ContentNode* nodeContent = dynamic_cast<const xmlpp::ContentNode*>(node);
  const xmlpp::TextNode* nodeText = dynamic_cast<const xmlpp::TextNode*>(node);
  const xmlpp::CommentNode* nodeComment = dynamic_cast<const xmlpp::CommentNode*>(node);

  if(nodeText && nodeText->is_white_space()) //Let's ignore the indenting - you don't always want to do this.
    return;
    
  const Glib::ustring nodename = node->get_name();

  if(!nodeText && !nodeComment && !nodename.empty()) //Let's not say "name: text".
  {
    const Glib::ustring namespace_prefix = node->get_namespace_prefix();

    std::cout << indent << "Node name = ";
    if(!namespace_prefix.empty())
      std::cout << namespace_prefix << ":";
    std::cout << nodename << std::endl;
  }
  else if(nodeText) //Let's say when it's text. - e.g. let's say what that white space is.
  {
    std::cout << indent << "Text Node" << std::endl;
  }

  //Treat the various node types differently: 
  if(nodeText)
  {
    std::cout << indent << "text = \"" << nodeText->get_content() << "\"" << std::endl;
        std::cout << "xpath" << nodeText->get_path() << std::endl;
  }
  else if(nodeComment)
  {
    std::cout << indent << "comment = " << nodeComment->get_content() << std::endl;
  }
  else if(nodeContent)
  {
    std::cout << indent << "content = " << nodeContent->get_content() << std::endl;
  }
  else if(const xmlpp::Element* nodeElement = dynamic_cast<const xmlpp::Element*>(node))
  {
    //A normal Element node:

    //line() works only for ElementNodes.
    std::cout << indent << "     line = " << node->get_line() << std::endl;

    //Print attributes:
    const xmlpp::Element::AttributeList& attributes = nodeElement->get_attributes();
    for(xmlpp::Element::AttributeList::const_iterator iter = attributes.begin(); iter != attributes.end(); ++iter)
    {
      const xmlpp::Attribute* attribute = *iter;
      const Glib::ustring namespace_prefix = attribute->get_namespace_prefix();

      std::cout << indent << "  Attribute ";
      if(!namespace_prefix.empty())
        std::cout << namespace_prefix  << ":";
      std::cout << attribute->get_name() << " = " << attribute->get_value() << std::endl;
    }

    const xmlpp::Attribute* attribute = nodeElement->get_attribute("title");
    if(attribute)
    {
      std::cout << indent << "title = " << attribute->get_value() << std::endl;
    }
  }
  
  if(!nodeContent)
  {
    //Recurse through child nodes:
    xmlpp::Node::NodeList list = node->get_children();
    for(xmlpp::Node::NodeList::iterator iter = list.begin(); iter != list.end(); ++iter)
    {
      print_node(*iter, indentation + 2); //recursive
    }
  }
}

std::string Utils::getQueueUrl(std::stringstream &ss) { //TODO check for memory leak
    try {
      xmlpp::DomParser parser;
      if (false)
        parser.set_validate();
      if (false)
        parser.set_throw_messages(false);
      parser.set_substitute_entities(true);
      parser.parse_stream(ss);
      if(parser) {
        for (auto node : parser.get_document()->get_root_node()->find("/*/*[1]/*/text()")) {
          const xmlpp::TextNode* nodeText = dynamic_cast<const xmlpp::TextNode*>(node);
          if (nodeText == nullptr)
            return "";
          return nodeText->get_content();
        }
      }
    }
    catch(const std::exception& ex) {
      std::cerr << "Exception caught: " << ex.what() << std::endl;
    }
    return "";
}


std::string Utils::getQueueCanonicalUri(std::string &queueUrl) { //Use regex when supported by the compiler
  std::string tmp(queueUrl);
  unsigned long idx;
  bool found = false;

  if ((idx = tmp.find_last_of("/")) !=  string::npos) {
    tmp = tmp.substr(0, idx - 1);
  }
 if ((idx = tmp.find_last_of("/")) !=  string::npos) {
    found = true;
    tmp = tmp.substr(0, idx - 1);
  }
  if (found == false){
    throw std::runtime_error("Can't find the canonical uri of the queue from[" + queueUrl + "]");
  }
  return queueUrl.substr(idx, queueUrl.size()) + "/";
}

std::list<std::pair<std::string, std::string>> Utils::getMessagesLst(std::stringstream &ss) {
     std::list<std::pair<std::string, std::string>> rep;

     // TODO: read the doc of libxml++ ^^'
  xmlpp::DomParser parser;
  if (false)
    parser.set_validate();
  if (false)
    parser.set_throw_messages(false);
  parser.set_substitute_entities(true);
  parser.parse_stream(ss);
  if(parser) {
    auto root_node = parser.get_document()->get_root_node();
    for (int i = 1; i <= 10; i++) {
      auto nodeMsg = root_node->find(Utils::sprintf("/*/*[1]/*[%]/*[1]/text()", i));
      auto nodeHandle = root_node->find(Utils::sprintf("/*/*[1]/*[%]/*[3]/text()", i));
      if (nodeMsg.size() == 0 || nodeHandle.size() == 0) {
        break;
      }
      const xmlpp::TextNode* nodeText = dynamic_cast<const xmlpp::TextNode*>(nodeMsg[0]);
      const xmlpp::TextNode* nodeText2 = dynamic_cast<const xmlpp::TextNode*>(nodeHandle[0]);
      if (nodeText == nullptr || nodeText2 == nullptr) {
        break;
      }
      rep.push_back(std::make_pair(Crypto::base64Decode(nodeText->get_content()), nodeText2->get_content()));
    }
  }
  return std::move(rep);
}

int Utils::getQueueSize(std::stringstream &ss) {
  xmlpp::DomParser parser;
  if (false)
    parser.set_validate();
  if (false)
    parser.set_throw_messages(false);
  parser.set_substitute_entities(true);
  parser.parse_stream(ss);
  if(parser) {
    auto node = parser.get_document()->get_root_node()->find("/*/*[1]/*/*[2]/text()");
    if (node.size() > 0) {
      const xmlpp::TextNode* nodeText = dynamic_cast<const xmlpp::TextNode*>(node[0]);
      if (nodeText != nullptr) {
        return std::stoi(nodeText->get_content());
      }
    }
  }
  return -1;
}


void Utils::executeRequest(const std::string &url, std::stringstream &ss) {
	curl_writer writer(ss);
	curl_easy easy(writer);
  easy.add(curl_pair<CURLoption,string>(CURLOPT_URL, url) );
  easy.add(curl_pair<CURLoption,long>(CURLOPT_FOLLOWLOCATION,1L));
  try {
      easy.perform();
  } 
  catch (curl_easy_exception error) {
      // If you want to get the entire error stack we can do:
      vector<pair<string,string>> errors = error.what();
      // Otherwise we could print the stack like this:
      error.print_traceback();
      // Note that the printing the stack will erase it
  }
}

static curl_easy _easy; //used to encode/decode stuff

std::string Utils::escape(std::string &url) {
    _easy.escape(url);
    return url;
}

std::string Utils::unescape(std::string &url) {
  _easy.unescape(url);
  return url;
}

