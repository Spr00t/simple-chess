#pragma once
#include <boost/asio/deadline_timer.hpp>
#include <functional>

class Timeout
{
public:
    typedef std::function<void()> TWaitHandler;
    Timeout(boost::asio::io_service & io_service);

    void async_wait(TWaitHandler handler);

    void async_wait(TWaitHandler handler, const boost::asio::deadline_timer::duration_type & duration) {
        timer.expires_from_now(duration);
        async_wait(handler);
    }

    void stop() {
        timer.cancel();
        wait_counter++;
    }

    boost::asio::deadline_timer::duration_type expires_from_now() const {
        return timer.expires_from_now();
    }
    std::size_t expires_from_now(const boost::asio::deadline_timer::duration_type & expiry_time) {
        return timer.expires_from_now(expiry_time);
    }



private:
    void privateHandler(int counter, const boost::system::error_code& error);
    boost::asio::deadline_timer timer;
    TWaitHandler handler;
    int wait_counter;
};
