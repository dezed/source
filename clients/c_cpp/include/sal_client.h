#pragma once

#include "sal_data.h"
#include "sal_node.h"
#include "sal_signal.h"

#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/HTTPSClientSession.h"
#include "Poco/StreamCopier.h"
#include "Poco/URI.h"
#include "Poco/UTF8String.h"

namespace sal
{
    using namespace std;
    using namespace exception;
    uint32_t SUPPORTED_API_VERSION = 2;

    /// Todo: authentication is not implemented
    class Client
    {

    public:
        // why public ?
        bool verify_https_cert;
        string auth_token;

        Client(const string host)
                : Client(host, true){};
        Client(const string host, const bool verify_https_cert)
        {

            // authentication attributes
            this->auth_required = false;
            this->auth_token = "";
            this->verify_https_cert = verify_https_cert;

            // set and inspect host
            this->set_host(host);
        };

        virtual ~Client(){};

        const string get_host() const
        {
            return this->host_uri.toString();
        };
        void set_host(const string host)
        {

            bool auth_required;
            Poco::URI uri = Poco::URI(host);

            try
            {
                // connect to server and request server properties
                Poco::JSON::Object::Ptr json = this->make_get_request(uri);

                // check server response is valid and store relevant server settings
                string name = json->getObject("service")->getValue<string>("name");
                if (name != "Simple Access Layer (SAL) Server")
                    throw exception::SALException(); // TODO: add a message to exception: "Could not identify a SAL
                                                     // server on the specified host:"

                Poco::JSON::Object::Ptr api = json->getObject("api");
                uint32_t version = api->getValue<uint32_t>("version");
                if (version != SUPPORTED_API_VERSION)
                    throw exception::SALException(); // TODO: add a message to exception: "The server is using a newer
                                                     // API than the client recognises, please update your client."

                auth_required = api->getValue<bool>("requires_auth");
            }
            catch (Poco::JSON::JSONException)
            {
                throw exception::SALException(); // todo: add a sensible message
            }

            // all good, update host state
            this->auth_required = auth_required;
            this->host_uri = uri;
            this->auth_uri = Poco::URI(uri, "auth/");
            this->data_uri = Poco::URI(uri, "data/");
        };

        const bool is_auth_required() const
        {
            return this->auth_required;
        };

        void authenticate(const string user, const string password){
            /* server side has not yet completed
            try {
                // todo: connect to server and request server properties
            }
            */

        };

        /// explore the node path and deserialize into NodeObject
        node::NodeObject::Ptr list(const string sal_path) const
        {
            // TODO:
            node::NodeObject::Ptr nobj = nullptr;
            Poco::URI node_uri(this->data_uri, sal_path);
            auto jobj = make_get_request(node_uri);
            if (jobj)
                return node::decode(jobj);
            return nobj;
        };

        /// deserialized into Signal or Dictionary data attribute/object
        /// `obj = sal.get(path, summary=False)`
        object::Attribute::Ptr get(const string path, bool summary = false) const
        {
            // TODO: convert sal path to uri
            string sal_path = path;

            // todo: kludge
            Poco::URI node_uri(this->data_uri, sal_path);

            cout << node_uri.toString() << endl;

            Poco::JSON::Object::Ptr obj = this->make_get_request(node_uri);


            obj->stringify(cout, 2); // debugging

            return object::decode(obj);
        };

        // if data type has been knowned from list(), directly cast to type

        /// push modified data to server,
        /// also needs to check permission
        void put(const string path, const node::NodeObject obj) const
        {
            // todo: first of all, make/register an explorable node  on the back end
            // then push full data to the backend and store
            throw SALException("this method is currently not supported by server backend.");
        };

        /// this works like copy file, providing the source path and target path
        /// todo: very weird API, switch T and S the parameter position
        void copy(const string target, const string source) const
        {
            // todo: send request, and check http STATUS
            throw SALException("this method is currently not supported by server backend.");
        }

        /// todo: user must have the permission to delete any node object
        void del(const string path) const
        {
            // todo: send request, and check http STATUS
            throw SALException("not implemented yet");
        };

    protected:
        bool auth_required;
        Poco::URI host_uri;
        Poco::URI auth_uri;
        Poco::URI data_uri;

        const Poco::SharedPtr<Poco::Net::HTTPClientSession> open_session(const Poco::URI uri) const
        {

            using namespace Poco::Net;

            // http or https?
            string scheme = uri.getScheme();
            if (scheme == "http")
            {

                // http session
                return new HTTPClientSession(uri.getHost(), uri.getPort());
            }
            else if (scheme == "https")
            {

                if (this->verify_https_cert)
                {
                    // https session, checking certificate is valid
                    return new HTTPSClientSession(uri.getHost(), uri.getPort());
                }
                else
                {
                    // https session, ignoring invalid certificates
                    return new HTTPSClientSession(uri.getHost(), uri.getPort(),
                                                  new Context(Context::CLIENT_USE, "", Context::VERIFY_NONE));
                }
            }
            else
                throw Poco::UnknownURISchemeException();
        };

        /// return nullptr if failed, embrace all code in try block
        Poco::JSON::Object::Ptr make_get_request(const Poco::URI uri) const
        {

            using namespace Poco;
            using namespace Poco::Net;
            using namespace Poco::JSON;

            HTTPResponse response;
            Poco::JSON::Parser parser;
            string json;

            // open http or https session
            SharedPtr<HTTPClientSession> session = this->open_session(uri);

            // redirect empty path to root
            string path = uri.getPathEtc();
            if (path.empty())
                path = "/";

            // make request

            HTTPRequest request(HTTPRequest::HTTP_GET, path, HTTPMessage::HTTP_1_1);
            session->sendRequest(request);
            // todo: handle errors: NodeNotFound
            StreamCopier::copyToString(session->receiveResponse(response), json);

            // decode json
            // todo: handle exceptions: parsing error, return nullptr if failed?
            return parser.parse(json).extract<Poco::JSON::Object::Ptr>();
        };

        Poco::JSON::Object::Ptr make_post_request(const Poco::URI uri) const
        {
            return nullptr;
        }

        Poco::JSON::Object::Ptr make_del_request(const Poco::URI uri) const
        {
            return nullptr;
        }

        //            string serialise(node::NodeObject obj);
        //            node::NodeObject deserialise(string json);
    };
} // namespace sal