// Authored by Alex Meakins 2019

#include "Poco/JSON/Parser.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/HTTPSClientSession.h"
#include "Poco/StreamCopier.h"
#include "Poco/URI.h"

#include <iostream>

using Poco::StreamCopier;
using Poco::URI;
using Poco::Net::HTTPMessage;
using Poco::Net::HTTPRequest;
using Poco::Net::HTTPResponse;
using Poco::Net::HTTPSClientSession;


int main(int argc, char** argv)
{

    // connect and setup request
    URI uri("https://sal.jet.uk/data/pulse/83373/ppf/signal/jetppf/magn/ipla?object=summary");
    HTTPSClientSession session(uri.getHost(), uri.getPort());
    HTTPRequest request(HTTPRequest::HTTP_GET, uri.getPathEtc(), HTTPMessage::HTTP_1_1);
    HTTPResponse response;

    // print request
    request.write(std::cout);

    // make request
    session.sendRequest(request);
    std::istream& rs = session.receiveResponse(response);

    // print request
    std::cout << response.getStatus() << " " << response.getReason() << std::endl;
    response.write(std::cout);
    StreamCopier::copyStream(rs, std::cout);
    std::cout << std::endl;
}