#include <ctime> 
#include <iostream>
#include <string>
#include <optional> 
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/execution_context.hpp> 
#include <boost/beast/http/status.hpp> 
#include <boost/beast/websocket/stream.hpp>     
#include <boost/beast/core/tcp_stream.hpp>     
#include <json.h> 
#include <thread>
#include "cheader.h"        
#include "json/json.h"          

 

 
using namespace boost::asio;
using ip::tcp; 
using std::string;   
using std::cout;
using std::endl;

string read_(tcp::socket& socket) 
{ 
    boost::asio::streambuf buf;
    boost::asio::read_until(socket, buf, "\n");
    string data = boost::asio::buffer_cast<const char*>(buf.data());
    return data;
}
void send_(tcp::socket& socket, const string& message)
{
    const string msg = message + "\n"; 
    boost::asio::write(socket, boost::asio::buffer(msg)); 
}

Json::Reader reader;  
Json::Value root;   

int fun_fox_server()
{
       
    
    

        boost::asio::io_service io_service;
    
        for (;;) {

            //listen for new connection
            tcp::acceptor acceptor_(io_service, tcp::endpoint(tcp::v4(), 1234));

            //socket creation
            tcp::socket socket_(io_service);

            //waiting for connection
            acceptor_.accept(socket_);
            //read operation
            string message = read_(socket_);
            cout << "String: " << message << endl;
            bool parseSuccess = reader.parse(message, root, false);
            string arr        = "       ";
            cout << "bool: " << parseSuccess << endl;

            if (parseSuccess) {
                const Json::Value resultValue = root["name"];
                cout << "Result is " << resultValue.asString() << "\n";
                string ket = resultValue.asString() + "\n";
                int len    = ket.length();
                cout << "Lenght of ket: " << len << endl;
                for (int i = 0; i < len; i++) {
                    arr.at(i) = ket.at(i);
                }
            }

            string up = "update\n";
            int num;
            int LEN = arr.length();
            for (int i = 0; i < LEN; i++) {
                if (arr.at(i) != up.at(i)) {
                    num = 0;
                    break;
                }
                num = 1;
            }
            cout << "num: " << num << endl;
            if (num == 0) {
                cout << "Strings not equal" << endl;
            } else if (num == 1) {
                cout << "Starting update..." << endl;
                char update[] = "update";
                char* ptr[]   = {update};
                int dc;
                dc = fun_up_scan(1, ptr);
                if (dc == 0) {
                    send_(socket_, "{\"name\":\"update\"}\n");
                } else if (dc == -1) {
                    send_(socket_, "{\"name\":\"update\"}\n");
                }
            }
            cout << "Servent sent message to client" << endl;
        } 
    return 0;
}






