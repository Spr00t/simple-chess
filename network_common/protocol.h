#pragma once

#include "message.h"

#include <memory>
#include <atomic>
#include <boost/asio.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/noncopyable.hpp>
#include <boost/lexical_cast.hpp>

class Protocol
{
public:
    typedef std::function<void ()> TReadyHandler;

    typedef std::function<void (const Message &)> TMessageHandler;

    Protocol(boost::asio::ip::tcp::socket socket);
//    void AsyncSendMessage(const Message & message, TReadyHandler handler);
//    void AsyncGetMessage(TMessageHandler handler);

    void AsyncSendMessage(Message message, TReadyHandler handler);
    void AsyncGetMessage(TMessageHandler handler);

private:
    void parseMessage(TMessageHandler, const boost::system::error_code& ec,
                      std::size_t bytes_transferred);

    int instance_id_ = 0;

    boost::asio::io_service::strand flow;

    boost::asio::ip::tcp::socket socket;
    boost::asio::streambuf response;
    boost::asio::streambuf request;
};
typedef std::shared_ptr<Protocol> ProtocolPtr;
typedef std::shared_ptr<boost::asio::io_service> TIoPtr;


