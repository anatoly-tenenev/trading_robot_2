#include <iostream>
#include <qsh/qsh.hpp>
#include <atrade/atrade.hpp>
#include <indicators/common/moex_indicator_2.hpp>
#include <strategies/common/all.hpp>
#include <functions/functions.hpp>

using namespace std;

using namespace atrade;
using atrade::backtester::SimpleBacktester;
using atrade::backtester::QshDataSource;

int main()
{
    asio::io_service ios;
    asio::io_service::work work(ios);

    auto backtester = SimpleBacktester(atrade::pt::milliseconds(2), std::bind(async2::_current_datetime, &ios));
    backtester.add_data_source<QshDataSource>("OrdLog.Si-12.15.2015-09-16.qsh");

    async2::init_market(ios, backtester);

    int64_t isin_id;
    async2::TradingContext ctx;

    asio::spawn(ios, [&] (asio::yield_context yield) {
        isin_id = async2::get_isin_id(ios, "Si-12.15", yield);

        SimpleTradingGuard2 full_strategy(ios, isin_id);
        full_strategy.set_time_indicator<MoexIndicator2>(ios, ctx);
        full_strategy.set_closing_strategy<SimpleClosing2>(ios, isin_id);
        full_strategy.add_strategy<LoopStrategy>(ios, isin_id);

        full_strategy(ctx, yield);
    });

    try
    {
        while (true)
        {
            async2::process_market(ios);
            ios.poll();
        }
    }
    catch(atrade::backtester::EofQshException&)
    {
        async2::Portfolio& portfolio = async2::get_portfolio(ios);
        std::cout << "profit: " << portfolio.profit(isin_id) << "\n";
        backtester.print_result();
    }

    return 0;
}


