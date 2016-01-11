#include <iostream>
#include <qsh/qsh.hpp>
#include <atrade/atrade.hpp>
#include <functions/functions.hpp>

using namespace atrade;

int main()
{
    using atrade::backtester::SimpleBacktester;
    using atrade::backtester::QshDataSource;
    using async2::TradingContext;
    using asio::yield_context;
    using asio::io_service;

    asio::io_service ios;
    asio::io_service::work work(ios);

    auto backtester = SimpleBacktester(atrade::pt::milliseconds(2), std::bind(async2::_current_datetime, &ios));
    backtester.add_data_source<QshDataSource>("OrdLog.Si-3.16.2015-12-23.qsh");

    async2::init_market(ios, backtester);

    auto subscr = async2::subscribe_ordlog(ios, [&](int64_t, const atrade::OrdLogEntry&) {
    });
    double price = 0;

    asio::spawn(ios, [&](yield_context yield) {
        int64_t isin_id = async2::get_isin_id(ios, "Si-3.16", yield);
        subscr = async2::subscribe_ordlog(ios, [&, isin_id](int64_t, const atrade::OrdLogEntry& entry) {
            OrderBook ob(ios, isin_id);
            double _price = (ob.best_bid() + ob.best_ask())/2;
            if (price == _price)
            {
                return;
            }
            price = _price;
            std::cout << entry.datetime << ";";
            std::cout << price << ";";
            for (int i=0; i<100; ++i)
            {
                std::cout << ob.asks(i) - ob.bids(i);
                if (i < 99)
                {
                    std::cout << ";";
                }
            }
            std::cout << "\n";
        });
    });

    while (true)
    {
        async2::process_market(ios);
        ios.poll();
    }

    return 0;
}


