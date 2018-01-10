#include "timeout.h"
#include <memory>

using namespace std;
Timeout::Timeout(boost::asio::io_service &io_service):
    timer(io_service)
{

}

void Timeout::async_wait(TWaitHandler handler)
{
    timer.cancel();
    this->handler = handler;
    wait_counter++;
    timer.async_wait(bind(&Timeout::privateHandler, this, wait_counter, placeholders::_1));
}

void Timeout::privateHandler(int counter, const boost::system::error_code &error)
{
    if (error != boost::asio::error::operation_aborted && counter == wait_counter && handler) {
        handler();
    }
}
