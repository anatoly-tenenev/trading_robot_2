#include <iostream>
#include <qsh/qsh.hpp>
#include <atrade/atrade.hpp>

using namespace std;

using atrade::backtester::SimpleBacktester;
using atrade::backtester::QshDataSource;
namespace asio = atrade::asio;
namespace pt = atrade::pt;
namespace async2 = atrade::async2;

int main()
{
    asio::io_service ios;
    asio::io_service::work work(ios);

    auto backtester = SimpleBacktester(atrade::pt::milliseconds(50), std::bind(async2::_current_datetime, &ios));
    backtester.add_data_source<QshDataSource>("OrdLog.GAZR-12.15.2015-09-17.qsh");
    backtester.add_data_source<QshDataSource>("OrdLog.MIX-12.15.2015-09-17.qsh");

    async2::init_market(ios, backtester);

//    atrade::async2::IAsyncMarket& market = atrade::async2::get_market(ios);

//    auto subscr = market.subscribe_tick([](const atrade::Deal& tick) {
//        std::cout << tick.datetime << " isin_id=" << tick.isin_id << "\n";
//    });


    size_t numb = 0;
    atrade::async2::Interval interval(ios, pt::milliseconds(1), [&]() {
        //std::cout << "interval:" << atrade::async2::current_datetime(ios) << "\n";
        ++numb;
    });

    asio::spawn(ios, [&ios] (asio::yield_context yield) {
        std::cout << "#1-1:" << atrade::async2::current_datetime(ios) << "\n";
        atrade::async2::Timer timer(ios, pt::hours(3));
        timer.async_wait(yield);
        std::cout << "#1-2:" << atrade::async2::current_datetime(ios) << "\n";
        timer.expires_at(timer.expires_at()+pt::hours(3));
        timer.async_wait(yield);
        std::cout << "#1-3:" << atrade::async2::current_datetime(ios) << "\n";
    });

    asio::spawn(ios, [&ios] (asio::yield_context yield) {
        std::cout << "#3-1:" << atrade::async2::current_datetime(ios) << "\n";
        atrade::async2::Timer timer(ios, pt::hours(2));
        timer.async_wait(yield);
        std::cout << "#3-2:" << atrade::async2::current_datetime(ios) << "\n";
        timer.expires_at(timer.expires_at()+pt::hours(2));
        timer.async_wait(yield);
        std::cout << "#3-3:" << atrade::async2::current_datetime(ios) << "\n";
    });

    asio::spawn(ios, [&ios] (asio::yield_context yield) {
        std::cout << "#2-1:" << atrade::async2::current_datetime(ios) << "\n";
        atrade::async2::Timer timer(ios, pt::hours(1));
        timer.async_wait(yield);
        std::cout << "#2-2:" << atrade::async2::current_datetime(ios) << "\n";
        timer.expires_at(timer.expires_at()+pt::hours(1));
        timer.async_wait(yield);
        std::cout << "#2-3:" << atrade::async2::current_datetime(ios) << "\n";
    });

    try
    {
        while (true)
        {
            async2::process_market(ios);
            ios.poll();
        }
    }
    catch(...)
    {
        std::cout << numb << "\n";
        throw;
    }

    return 0;
}

