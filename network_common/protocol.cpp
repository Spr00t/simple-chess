#include "protocol.h"
#include "global.h"

#include <exception>
#include <boost/format.hpp>

using namespace std;
using namespace boost;

static std::atomic<int> instance_counter(0);


Protocol::Protocol(boost::asio::ip::tcp::socket socket):
    socket(std::move(socket)),
    flow(socket.get_io_service())
{
    instance_id_ = instance_counter++;
}


void Protocol::AsyncSendMessage(Message message, Protocol::TReadyHandler handler)
{
    std::ostream request_stream(&request);
    request_stream << message;

    Global::instance().log(str(format("ID: %1% Message sent: %2%") % instance_id_ % message));
    async_write(socket, request, [handler](const boost::system::error_code& err, std::size_t bytes_transferred) {
        if (!err) {
            handler();
        }
    });

}

void Protocol::AsyncGetMessage(Protocol::TMessageHandler handler)
{
    async_read_until(socket,
                     response,
                     "\n",
                     std::bind(&Protocol::parseMessage, this, handler,
                      std::placeholders::_1,
                      std::placeholders::_2));
}

void Protocol::parseMessage(Protocol::TMessageHandler messageHandler, const boost::system::error_code &ec, std::size_t /*bytes_transferred*/)
{
    if (NOT ec) {
        std::istream response_stream(&response);
        std::string text;
        std::getline(response_stream, text);

        Global::instance().log(str(format("ID: %1% Message received: %2%") % instance_id_ % text));

        TOptionalMessage message = Message::fromString(text);
        if (NOT message)
            throw runtime_error("");
        messageHandler(*message);
    }
}
