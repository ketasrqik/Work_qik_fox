//#include <cstdio>
//#include <ctime> 
//#include <iostream>
//#include <string>
//#include <optional> 
//#include <boost/bind.hpp>
//#include <boost/shared_ptr.hpp>
//#include <boost/enable_shared_from_this.hpp>
//#include <boost/asio.hpp>
//#include <boost/asio/io_service.hpp>
//#include <boost/asio/write.hpp>
//#include <boost/asio/buffer.hpp>
//#include <boost/asio/ip/tcp.hpp>
//#include <boost/asio/execution_context.hpp> 
//#include <boost/beast/http/status.hpp> 
//#include <boost/beast/websocket/stream.hpp>   
//#include <boost/beast/core/tcp_stream.hpp>           
//#include <json.h> 
//#include <thread>
//#include "cheader.h"        
//#include "json/json.h"        
/*#include "platform.h"   */     
//#include <buffers_to_string.hpp>
//#include <qsort.c>  
//
//
////namespace beast     = boost::beast;         // from <boost/beast.hpp>
////namespace http      = beast::http;          // from <boost/beast/http.hpp>
////namespace websocket = beast::websocket;     // from <boost/beast/websocket.hpp>
////namespace net       = boost::asio;          // from <boost/asio.hpp>
////using tcp           = boost::asio::ip::tcp; // from <boost/asio/ip/tcp.hpp>
//
//
//
//using namespace boost::asio;
//using ip::tcp;
//using std::string;   
//using std::cout;
//using std::endl;

//int main()
//{
//
//    auto const address = net::ip::make_address("127.0.0.1");
//    auto const port    = static_cast<unsigned short>(std::atoi("8083"));
//
//    net::io_context ioc{1};
//
//    tcp::acceptor acceptor{ioc, {address, port}};
//    for (;;) {
//
//        tcp::socket socket{ioc};    
//
//        acceptor.accept(socket);
//
//        std::thread{std::bind(
//                        //[q = std::move(socket)]() mutable { // socket will be const - mutable should be used
//                        [q{std::move(socket)}]() { // socket will be const - mutable should be used
//                            websocket::stream<tcp::socket> ws{std::move(const_cast<tcp::socket&>(q))};
//
//                            // Set a decorator to change the Server of the handshake
//                            // no need to set. It ıs not necessary
//                            ws.set_option(websocket::stream_base::decorator(
//                                [](websocket::response_type& res) {
//                                    res.set(http::field::server,
//                                            std::string(BOOST_BEAST_VERSION_STRING) +
//                                                " websocket-server-sync");
//                                }));
//
//                            // Accept the websocket handshake
//                            ws.accept();
//
//                            while (true) {
//                                try {
//
//                                    // This buffer will hold the incoming message
//                                    // buffer types https://www.boost.org/doc/libs/1_75_0/libs/beast/doc/html/beast/using_io/buffer_types.html
//                                    // check for the best one
//                                    //beast::multi_buffer buffer;
//                                    beast::flat_buffer buffer;
//
//                                    // Read a message
//                                    ws.read(buffer);
//
//                                    auto out = beast::buffers_to_string(buffer.cdata());   
//                                    std::cout << out << std::endl;
//
//                                    // Echo the message back
//                                    //ws.text(ws.got_text());
//                                    //bost::beast::ostream(buffer) << "something";
//                                    ws.write(buffer.data());
//                                    if (beast::buffers_to_string(buffer.data()) == "dbupdate") {
//
//                                        fun_up_scan(); 
//                                        ws.write(net::buffer(std::string("DB updated Successfully")));
//                                    }
//
//                                } catch (beast::system_error const& se) {
//                                    if (se.code() != websocket::error::closed) {
//                                        std::cerr << "Error: " << se.code().message() << std::endl;
//                                        break;
//                                    }
//                                }
//                            }
//                        })}
//            .detach();
//    }
//}


//string read_(tcp::socket& socket)         
//{ 
//    cout << "Before streambuf" << endl;    
//    boost::asio::streambuf buf;
//    cout << "After streambuf" << endl;
//    boost::asio::read_until(socket, buf, "\n");      
//    cout << "After read_until" << endl; 
//    string data = boost::asio::buffer_cast<const char*>(buf.data());
//    cout << "After buffer_cast" << endl; 
//    return data; 
//}
//void send_(tcp::socket& socket, const string& message)
//{
//    cout << "Entered send" << endl;
//    const string msg = message + "\n";
//    cout << "string msg" << endl;
//    boost::asio::write(socket, boost::asio::buffer(msg)); 
//    cout << "After write" << endl;
//}

//string read_(tcp::socket& socket)
//{
//    boost::asio::streambuf buf;
//    boost::asio::read_until(socket, buf, "\n");
//    string data = boost::asio::buffer_cast<const char*>(buf.data());
//    return data;
//}
//void send_(tcp::socket& socket, const string& message)
//{
//    const string msg = message + "\n";
//    boost::asio::write(socket, boost::asio::buffer(message));
//}
//
//int main()
//{
//    boost::asio::io_service io_service;
//    //listen for new connection
//    tcp::acceptor acceptor_(io_service, tcp::endpoint(tcp::v4(), 1234));
//    //socket creation
//    tcp::socket socket_(io_service);        
//    //waiting for connection
//    acceptor_.accept(socket_);
//    //read operation 
//    string message = read_(socket_);    
//    cout << message << endl;
//    //write operation
//    send_(socket_, "Hello From Server!");
//    cout << "Servent sent Hello message to Client!" << endl;
//    return 0;  
//}

//int main()
//{
//    Json::Reader reader;
//    Json::Value root;   
//    
//   
//        boost::asio::io_service io_service;
//        //listen for new connection
//        tcp::acceptor acceptor_(io_service, tcp::endpoint(tcp::v4(), 8000));
//        cout << "Port provided" << endl;
//        //socket creation
//        tcp::socket socket_(io_service);  
//        cout << "Socket created" << endl;   
//        // accepting new request  
//        /*boost::beast::websocket::stream<boost::beast::tcp_stream> ws(acceptor_.accept());*/  
//        acceptor_.accept(socket_);   
//
//        std::thread
//        {std::bind(
//                        //[q = std::move(socket)]() mutable { // socket will be const - mutable should be used
//                        [q{std::move(socket_)}]() { // socket will be const - mutable should be used
//            boost::beast::websocket::stream<tcp::socket> ws{std::move(const_cast<tcp::socket&>(q))};  
//
//        cout << "After accepting request" << endl;     
//        // This buffer is required for reading HTTP messages  
//        boost::beast::flat_buffer buffer;  
//        cout << "After buffer" << endl; 
//        // Read the HTTP request ourselves
//        boost::beast::http::request<boost::beast::http::string_body> req;      
//        cout << "After req" << endl; 
//        boost::beast::http::read(socket_, buffer, req);   
//        cout << "After read http" << endl;  
//        // See if its a WebSocket upgrade request
//        if (boost::beast::websocket::is_upgrade(req)) { 
//            cout << "After if check" << endl; 
//            // Construct the stream, transferring ownership of the socket
//            boost::beast::websocket::stream<boost::beast::tcp_stream> ws(std::move(socket_));    
//            cout << "After stream" << endl; 
//            // Clients SHOULD NOT begin sending WebSocket
//            // frames until the server has provided a response.
//            BOOST_ASSERT(buffer.size() == 0); 
//            cout << "After BOOST_ASSERT" << endl;  
//            // Accept the upgrade request
//            ws.accept(req);        
//            cout << "After ws.accept(req)" << endl;      
//        } else { 
//            cout << "No upgrade request" << endl;
//        }
//        /*acceptor_.accept(socket_); */
//        read operation
//        cout << "Handshake done\n"<< endl; 
//           
//        cout << "After read" << endl; 
//
//        string message = read_(socket_);         
//        cout << "String: " << message << endl;
//        // json start 
//        bool parseSuccess = reader.parse(message, root, false); 
//        string arr        = "       "; 
//        cout << "bool: " << parseSuccess<<endl;
//
//        if (parseSuccess) { 
//            const Json::Value resultValue = root["name"]; 
//            cout << "Result is " << resultValue.asString() << "\n"; 
//            string ket = resultValue.asString() + "\n";
//            int len = ket.length();
//            cout << "Lenght of ket: " << len << endl;
//            for (int i = 0; i < len; i++) {
//                arr.at(i) = ket.at(i);    
//            }
//        }
//
//        string up = "update\n"; 
//        int num;
//        int LEN = arr.length();
//        for (int i = 0; i < LEN; i++) {
//            if (arr.at(i) != up.at(i)) {
//                num = 0;
//                break;
//            }
//            num = 1;
//        }
//        cout << "num: " << num << endl;
//        if (num == 0) {
//            cout << "Strings not equal" << endl;
//        } else if (num == 1) {
//            cout << "Starting update..." << endl;
//            char update[] = "update";
//            char* ptr[]   = {update}; 
//            int dc;
//            dc = fun_up_scan(1, ptr);
//            if (dc == 0) {
//                send_(socket_, "{\"name\":\"Successful\"}");
//            } else if (dc == -1) {
//                send_(socket_, "{\"name\":\"Unsuccessful\"}");
//            }
//        }
//        cout << "Servent sent message to client" << endl;
//    
//        io_service.run();
//    
//    return 0;
//}


//using namespace boost::asio;
//using namespace boost::asio::ip;
//using std::string;
//using std::cout;
//using std::endl;
//
//boost::asio::io_service ioservice;
//tcp::endpoint tcp_endpoint{tcp::v4(), 8010};
//tcp::acceptor tcp_acceptor{ioservice, tcp_endpoint};
//tcp::socket tcp_socket{ioservice};
//std::string message;
//std::string ket; 
//std::string arr;
//std::string asr;
//Json::Reader reader;
//Json::Value root;  
//
////void send_(tcp::socket& socket, const string& message)
////{
////    const string msg = message + "\n";
////    boost::asio::write(socket, boost::asio::buffer(msg)); 
////}
//
//string read_(tcp::socket& socket) 
//{
//    boost::asio::streambuf buf;
//    boost::asio::read_until(socket, buf, "r\n\r\n");
//    string data = boost::asio::buffer_cast<const char*>(buf.data());
//    return data;
//}
//
//void write_handler(const boost::system::error_code &ec,
//                   std::size_t bytes_transferred)
//{
//    if (!ec)
//        tcp_socket.shutdown(tcp::socket::shutdown_send);
//}
//
//void accept_handler(const boost::system::error_code& ec)
//{
//    cout << "In server" << endl;
//    if (!ec) {
//        message = read_(tcp_socket);
//        cout << "String: " << message << endl;
//        bool parseSuccess = reader.parse(message, root, false);
//        arr               = "       ";
//        cout << "bool: " << parseSuccess << endl;
//
//        if (parseSuccess) {
//            const Json::Value resultValue = root["name"];
//            cout << "Result is " << resultValue.asString() << "\n";
//            asr     = resultValue.asString() + "\n";
//            int len = asr.length();
//            cout << "Lenght of ket: " << len << endl;
//            for (int i = 0; i < len; i++) {
//                arr.at(i) = asr.at(i);
//            }
//        }
//
//        string up = "update\n";
//        int num;
//        int LEN = arr.length();
//        for (int i = 0; i < LEN; i++) {
//            if (arr.at(i) != up.at(i)) {
//                num = 0;
//                break;
//            }
//            num = 1;
//        }
//        cout << "num: " << num << endl;
//        if (num == 0) {
//            cout << "Strings not equal" << endl;
//            /*send_(tcp_socket, "{\"name\":\"Successful\"}");*/
//        } else if (num == 1) {
//            cout << "Starting update..." << endl;
//            char update[] = "update";
//            char* ptr[]   = {update};
//            int dc;
//            dc = fun_up_scan(1, ptr);
//            if (dc == 0) {
//                ket = "{\"name\":\"Successfull\"}\n";
//                async_write(tcp_socket, boost::asio::buffer(ket), write_handler);
//            } else if (dc == -1) {
//                ket = "{\"name\":\"UnSuccessfull\"}\n";
//                async_write(tcp_socket, boost::asio::buffer(ket), write_handler);
//            }
//        }
//        cout << "Servent sent message to client" << endl;
//    }
//}  
//
//int main()
//{
//    // Bind to ip address
//    
//    
//    // listen is called to make acceptor start listening
//    tcp_acceptor.listen();
//
//    boost::beast::websocket::stream<boost::beast::tcp_stream> ws(tcp_acceptor.accept()); 
//    ws.accept();     
//
//    ioservice.run(); 
//}