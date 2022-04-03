#include <cstdio>

#include <boost/beast/core.hpp>  
#include <boost/beast/websocket.hpp>   
#include <boost/math/tools/lexical_cast.hpp>   
#include <boost/json.hpp> 
            
#include <iostream>
 
#include <string>    
#include <thread>   
#include "cheader.h"  
#include "json/json.h"             


//#undef main

// g++ -I /usr/include/boost -pthread websocket.cpp
namespace beast     = boost::beast;         // from <boost/beast.hpp>
namespace http      = beast::http;          // from <boost/beast/http.hpp>
namespace websocket = beast::websocket;     // from <boost/beast/websocket.hpp>
namespace net       = boost::asio;          // from <boost/asio.hpp>
using tcp           = boost::asio::ip::tcp; // from <boost/asio/ip/tcp.hpp>
using std::string;
using std::cout;
using std::endl; 

//------------------------------------------------------------------------------
Json::Reader reader; 
Json::Value root; 
string cmp, par; 
int num, pun, ftr;


class qik_Json 
{
  public:
    string formatJSON(string command, string message)
    {
        boost::json::object obj;
        obj["command"] = command; 
        obj["message"] = message;   
        string pqr     = boost::json::serialize(obj);  
        return pqr; 
    }
};

int main()
{
   

    auto const address = net::ip::make_address("127.0.0.1");
    auto const port    = static_cast<unsigned short>(std::atoi("8083"));

    net::io_context ioc{1};

    tcp::acceptor acceptor{ioc, {address, port}}; 
    for (;;) {

        tcp::socket socket{ioc};

        acceptor.accept(socket); 

        std::thread{std::bind(
                        //[q = std::move(socket)]() mutable { // socket will be const - mutable should be used
                        [q{std::move(socket)}]() { // socket will be const - mutable should be used
                            websocket::stream<tcp::socket> ws{std::move(const_cast<tcp::socket&>(q))}; 
                             
                            // Set a decorator to change the Server of the handshake
                            // no need to set. It ıs not necessary
                            ws.set_option(websocket::stream_base::decorator(
                                [](websocket::response_type& res) { 
                                    res.set(http::field::server,
                                            std::string(BOOST_BEAST_VERSION_STRING) +
                                                " websocket-server-sync");
                                }));

                            // Accept the websocket handshake 
                            ws.accept();  
                            qik_Json j_obj;     

                            while (true) { 
                                try {

                                    // This buffer will hold the incoming message
                                    // buffer types https://www.boost.org/doc/libs/1_75_0/libs/beast/doc/html/beast/using_io/buffer_types.html
                                    // check for the best one
                                    //beast::multi_buffer buffer;
                                    
                                    beast::flat_buffer buffer; 

                                    // Read a message
                                    ws.read(buffer);

                                    auto out = beast::buffers_to_string(buffer.cdata());
                                    std::cout << out << std::endl;
                                    bool parseSuccess = reader.parse(out, root, false);  
                                    cout << "bool: " << parseSuccess << endl; 
                                    if (parseSuccess) {
                                          const Json::Value resultValue = root["command"];
                                          const Json::Value msgValue    = root["message"];
                                          cout << "Result is " << resultValue.asString() << endl;
                                          cout << "Result is " << msgValue.asString() << endl; 
                                          cmp = resultValue.asString();
                                          par = msgValue.asString();  
                                            
                                    } 
                                      
                                    cout << "cmp: " << cmp << endl;
                                    cout << "par: " << par << endl;
                                    
                                    string up = "update"; 
                                    int LEN = cmp.length(); 
                                    for (int i=0; i<LEN; i++) {
                                        if (cmp.at(i) != up.at(i)) {
                                            num = 0; 
                                            break;
                                        }
                                        num = 1;
                                    }

                                    cout << "num: " << num << endl;
                                     
                                    string pqr = "started";
                                    int len    = par.length();
                                    for (int j = 0; j < len; j++) {
                                        if (par.at(j) != pqr.at(j)) {
                                            pun = 0;
                                            break;
                                        }
                                        pun = 1;
                                    }

                                    cout << "pun: " << pun << endl;
                                    // Echo the message back
                                    //ws.text(ws.got_text());
                                    //bost::beast::ostream(buffer) << "something";
                                    /*ws.write(buffer.data());*/
                                   
                                    if (num==1 && pun==1) {
                                        
                                        char update[] = "update";
                                        char* ptr[]   = {update};
                                        int dc;
                                        dc = fun_up_scan(1, ptr);
                                        
                                        if (dc == 0) {
                                            string ind = "DB_Update Successfull";
                                            string bht = "update";
                                            ws.write(net::buffer(j_obj.formatJSON(bht, ind))); 
                                                           
                                        } else if (dc == -1) {
                                            string ind = "Unsuccessful";
                                            string bht  = "update";  
                                            ws.write(net::buffer(j_obj.formatJSON(bht, ind)));  
                                        }

                                    } else {
                                        char scan[500];
                                        int len = cmp.length();
                                        memset(scan, 0, 500);
                                        for (int k = 0; k < len; k++) {
                                            scan[k] = cmp.at(k);
                                        }
                                        cout << "Scan: " << scan << endl;
                                        char* str[] = {scan};
                                        int kc;
                                        kc = fun_up_scan(1, str);
                                        if (kc == 7) {
                                            string ind = "Cant initialize libclamav";
                                            string bht  = "scan";
                                            ws.write(net::buffer(j_obj.formatJSON(bht, ind)));   
                                            
                                        } else if (kc == 8) {
                                            string ind = "Cant initialize antivirus engine";
                                            string bht  = "scan";
                                            ws.write(net::buffer(j_obj.formatJSON(bht, ind))); 
                                            
                                        } else if (kc == 9) {
                                            string ind = "cl_load error ";
                                            string bht  = "scan";
                                            ws.write(net::buffer(j_obj.formatJSON(bht, ind))); 
                                            
                                        } else if (kc == 10) {
                                            string ind = "Building engine failed";
                                            string bht  = "scan";
                                            ws.write(net::buffer(j_obj.formatJSON(bht, ind)));
                                            
                                        } else if (kc == 2) {
                                            string ind = "Virus detected";
                                            string bht  = "scan";
                                            ws.write(net::buffer(j_obj.formatJSON(bht, ind)));  
                                            
                                        } else if (kc == 3) {
                                            string ind = "No virus detected";
                                            string bht  = "scan";
                                            ws.write(net::buffer(j_obj.formatJSON(bht, ind)));   

                                        } else if (kc == 4) {
                                            string ind = "Error in file";
                                            string bht  = "scan";
                                            ws.write(net::buffer(j_obj.formatJSON(bht, ind)));   
                                        }
                                         
                                    }

                                    
                                } catch (beast::system_error const& se) {
                                    if (se.code() != websocket::error::closed) {
                                        std::cerr << "Error: " << se.code().message() << std::endl;
                                        break;
                                    }
                                }
                            }
                        })}
            .detach();
    }
}


