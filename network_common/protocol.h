#pragma once

#include "message.h"

#include <memory>
#include <atomic>
#include <boost/asio.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/noncopyable.hpp>
#include <boost/lexical_cast.hpp>

class Protocol;

typedef std::shared_ptr<Protocol> ProtocolPtr;
typedef std::weak_ptr<Protocol> ProtocolWeakPtr;
class Protocol : public std::enable_shared_from_this<Protocol>
{
public:

    typedef std::function<void ()> TReadyHandler;
    typedef std::function<void ()> TErrorHandler;


    typedef std::function<void (const Message &)> TMessageHandler;

    Protocol(boost::asio::ip::tcp::socket socket);
    void subscribeErrorHappened(TErrorHandler handler);


//    void AsyncSendMessage(const Message & message, TReadyHandler handler);
//    void AsyncGetMessage(TMessageHandler handler);

    void AsyncSendMessage(Message message, TReadyHandler handler);
    void AsyncGetMessage(TMessageHandler handler);

private:
    static void staticParseMessage(ProtocolWeakPtr weak, TMessageHandler h, const boost::system::error_code& ec,
                      std::size_t bytes_transferred) {
        ProtocolPtr ptr(weak);
        if (ptr) {
            ptr->parseMessage(h, ec, bytes_transferred);
        }
    }

    void parseMessage(TMessageHandler, const boost::system::error_code& ec,
                      std::size_t bytes_transferred);

    int instance_id_ = 0;
    TErrorHandler error_handler_;
    boost::asio::io_service::strand flow;

    boost::asio::ip::tcp::socket socket;
    boost::asio::streambuf response;
    boost::asio::streambuf request;
};


typedef std::shared_ptr<boost::asio::io_service> TIoPtr;


