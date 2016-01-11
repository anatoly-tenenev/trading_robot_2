#include <atrade/backtester/simplemarketsimulator.h>
#include "fake_market_data_source.h"
#include <gtest/gtest.h>

//std::ostream& operator<<(std::ostream& out, const Order& o)
//{
//    out << "order={" <<
//           "isin=" << o.isin_id <<
//           ",id=" << o.order_id <<
//           ",dir=" << static_cast<int>(o.dir) <<
//           ",price=" << o.price <<
//           ",vol=" << o.volume <<
//           ",type=" << static_cast<int>(o.type) <<
//           ",status=" << static_cast<int>(o.status) <<
//           ",did=" << o.deal_id <<
//           ",dprice=" << o.deal_price <<
//           ",rest=" << o.amount_rest;
//    return out;
//}

class SimpleMarketSimulatorTest : public testing::Test
{
protected:
    virtual void SetUp()
    {
        data_source = new FakeMarketDataSource;
        simulator.add_data_source(std::unique_ptr<backtester::IMarketDataSource>(data_source));
        simulator.order_handler([this](const Order& order) {
            orders.push_back(order);
        });
        simulator.order_error_handler([this](const OrderError& entry) {
            order_errors.push_back(entry);
        });
//        simulator.stock_handler([](int64_t isin_id, const std::vector<Quote>& stocks) {
//        });
    }

    virtual void TearDown()
    {
    }

    backtester::SimpleMarketSimulator simulator;
    FakeMarketDataSource* data_source;
    std::vector<Order> orders;
    std::vector<OrderError> order_errors;
};

TEST_F(SimpleMarketSimulatorTest, add_order)
{
    data_source->send_stocks(0, std::vector<BQuote>({ BQuote(11, 1, QuoteType::Ask) }));
    Order order(0, 1, OrderDir::Bid, 100.0, 2);
    simulator.add_order(order);
    order.type = OrderType::Limit;
    order.status = OrderStatus::Add;
    order.amount_rest = order.volume;
    std::vector<Order> _orders = {order};
    EXPECT_EQ(orders, _orders);
    order.volume = 3;
    simulator.add_order(order);
    order.amount_rest = order.volume;
    _orders.push_back(order);
    EXPECT_EQ(orders, _orders);
}

TEST_F(SimpleMarketSimulatorTest, send_stocks)
{
    data_source->send_stocks(0,
                std::vector<BQuote>({ BQuote(90, 5, QuoteType::Ask),
                                      BQuote(70, 5, QuoteType::Bid) }));
}

TEST_F(SimpleMarketSimulatorTest, add_order_with_stocks_1)
{
    data_source->send_stocks(0,
                std::vector<BQuote>({ BQuote(90, 5, QuoteType::Ask),
                                      BQuote(70, 5, QuoteType::Bid) }));
    Order order(0, 1, OrderDir::Bid, 700.0, 2);
    simulator.add_order(order);
    order.type = OrderType::Limit;
    order.status = OrderStatus::Add;
    order.amount_rest = order.volume;
    std::vector<Order> _orders = {order};
    EXPECT_EQ(orders, _orders);
}

TEST_F(SimpleMarketSimulatorTest, add_order_with_stocks_2)
{
    data_source->send_stocks(0,
                std::vector<BQuote>({ BQuote(90, 5, QuoteType::Ask),
                                      BQuote(70, 5, QuoteType::Bid) }));
    Order order(0, 1, OrderDir::Bid, 800.0, 2);
    simulator.add_order(order);
    order.type = OrderType::Limit;
    order.status = OrderStatus::Add;
    order.amount_rest = order.volume;
    std::vector<Order> _orders = {order};
    EXPECT_EQ(orders, _orders);
}

TEST_F(SimpleMarketSimulatorTest, buy_1)
{
    data_source->send_stocks(0,
                std::vector<BQuote>({ BQuote(90, 5, QuoteType::Ask),
                                      BQuote(70, 5, QuoteType::Bid) }));
    Order order(0, 1, OrderDir::Bid, 900.0, 2);
    simulator.add_order(order);
    order.type = OrderType::Limit;
    order.status = OrderStatus::Add;
    order.amount_rest = order.volume;
    std::vector<Order> _orders = {order};
    order.status = OrderStatus::Fill;
    order.deal_price = order.price;
    order.amount_rest = 0;
    _orders.push_back(order);
    EXPECT_EQ(orders, _orders);
}

TEST_F(SimpleMarketSimulatorTest, buy_2)
{
    data_source->send_stocks(0,
                std::vector<BQuote>({ BQuote(101, 1, QuoteType::Ask),
                                      BQuote(90, 5, QuoteType::Ask),
                                      BQuote(70, 5, QuoteType::Bid) }));
    Order order(0, 1, OrderDir::Bid, 1000.0, 8);
    simulator.add_order(order);
    order.type = OrderType::Limit;
    order.status = OrderStatus::Add;
    order.amount_rest = order.volume;
    std::vector<Order> _orders = {order};
    order.volume = 5;
    order.status = OrderStatus::Fill;
    order.deal_price = 900.0;
    order.amount_rest = 3;
    _orders.push_back(order);
    EXPECT_EQ(orders, _orders);
}

TEST_F(SimpleMarketSimulatorTest, buy_3)
{
    data_source->send_stocks(0, std::vector<BQuote>({ BQuote(101, 1, QuoteType::Ask) }));
    Order order(0, 1, OrderDir::Bid, 1000.0, 8);
    simulator.add_order(order);
    data_source->send_stocks(0,
                std::vector<BQuote>({ BQuote(90, 1, QuoteType::Ask),
                                      BQuote(70, 1, QuoteType::Bid) }));
    order.type = OrderType::Limit;
    order.status = OrderStatus::Add;
    order.amount_rest = order.volume;
    std::vector<Order> _orders = {order};
//    order.volume = 8;
//    order.status = OrderStatus::Fill;
//    order.deal_price = 1000.0;
//    order.amount_rest = 0;
//    _orders.push_back(order);
    EXPECT_EQ(orders, _orders);
}

TEST_F(SimpleMarketSimulatorTest, buy_4)
{
    data_source->send_stocks(0, std::vector<BQuote>({ BQuote(101, 1, QuoteType::Ask) }));
    Order order(0, 1, OrderDir::Bid, 1000.0, 8);
    simulator.add_order(order);
    data_source->send_stocks(0,
                std::vector<BQuote>({ BQuote(100, 1, QuoteType::Ask),
                                      BQuote(70, 1, QuoteType::Bid) }));
    order.type = OrderType::Limit;
    order.status = OrderStatus::Add;
    order.amount_rest = order.volume;
    std::vector<Order> _orders = {order};
//    order.volume = 8;
//    order.status = OrderStatus::Fill;
//    order.deal_price = 1000.0;
//    order.amount_rest = 0;
//    _orders.push_back(order);
    EXPECT_EQ(orders, _orders);
}

TEST_F(SimpleMarketSimulatorTest, buy_5)
{
    data_source->send_stocks(0,
                std::vector<BQuote>({ BQuote(101, 1, QuoteType::Ask),
                                      BQuote(90, 5, QuoteType::Ask),
                                      BQuote(70, 5, QuoteType::Bid) }));
    Order order(0, 1, OrderDir::Bid, 1000.0, 8);
    simulator.add_order(order);
    data_source->send_stocks(0,
                std::vector<BQuote>({ BQuote(90, 5, QuoteType::Ask) }));
    order.type = OrderType::Limit;
    order.status = OrderStatus::Add;
    order.amount_rest = order.volume;
    std::vector<Order> _orders = {order};
    order.volume = 5;
    order.status = OrderStatus::Fill;
    order.deal_price = 900.0;
    order.amount_rest = 3;
    _orders.push_back(order);
//    order.volume = 3;
//    order.status = OrderStatus::Fill;
//    order.deal_price = 1000.0;
//    order.amount_rest = 0;
//    _orders.push_back(order);
    EXPECT_EQ(orders, _orders);
}

TEST_F(SimpleMarketSimulatorTest, buy_6)
{
    data_source->send_stocks(0,
                std::vector<BQuote>({ BQuote(101, 1, QuoteType::Ask),
                                      BQuote(70, 8, QuoteType::Bid) }));
    Order order(0, 1, OrderDir::Bid, 700.0, 2);
    simulator.add_order(order);
    data_source->send_stocks(0,
                std::vector<BQuote>({ BQuote(70, 1, QuoteType::Bid) }));
    data_source->send_tick({0, 0, 70, 1, DealType::Sell});
    order.type = OrderType::Limit;
    order.status = OrderStatus::Add;
    order.amount_rest = order.volume;
    std::vector<Order> _orders = {order};
    EXPECT_EQ(orders, _orders);
    data_source->send_tick({0, 0, 70, 1, DealType::Sell});
//    order.volume = 1;
//    order.status = OrderStatus::Fill;
//    order.deal_price = 700.0;
//    order.amount_rest = 1;
//    _orders.push_back(order);
    EXPECT_EQ(orders, _orders);
}

TEST_F(SimpleMarketSimulatorTest, order_error_1)
{
    data_source->send_stocks(0,
                std::vector<BQuote>({ BQuote(90, 5, QuoteType::Ask),
                                      BQuote(70, 5, QuoteType::Bid) }));
    Order order(0, 1, OrderDir::Ask, 1000.0, 2);
    simulator.add_order(order);
    order.type = OrderType::Limit;
    order.status = OrderStatus::Add;
    order.amount_rest = order.volume;
    std::vector<Order> _orders = {order};
    order.dir = OrderDir::Bid;
    simulator.add_order(order);
    _orders.push_back(order);
    order.status = OrderStatus::Fill;
    order.deal_price = 900.0;
    order.amount_rest = 0;
    _orders.push_back(order);
    EXPECT_EQ(orders, _orders);
}

TEST_F(SimpleMarketSimulatorTest, order_error_2)
{
    data_source->send_stocks(0,
                std::vector<BQuote>({ BQuote(90, 5, QuoteType::Ask),
                                      BQuote(70, 5, QuoteType::Bid) }));
    Order order(0, 1, OrderDir::Ask, 900.0, 2);
    simulator.add_order(order);
    order.type = OrderType::Limit;
    order.status = OrderStatus::Add;
    order.amount_rest = order.volume;
    std::vector<Order> _orders = {order};
    order.dir = OrderDir::Bid;
    order.order_id = 2;
    simulator.add_order(order);
    EXPECT_EQ(orders, _orders);
    OrderError error;
    error.order_id = 2;
    error.op_type = OrderOperationType::Add;
    error.code = 0;
    std::vector<OrderError> errors = {error};
    EXPECT_EQ(order_errors, errors);
}

TEST_F(SimpleMarketSimulatorTest, add_tick_1)
{
    data_source->send_stocks(0,
                std::vector<BQuote>({ BQuote(90, 5, QuoteType::Ask),
                                      BQuote(70, 5, QuoteType::Bid) }));
    Order order(0, 1, OrderDir::Bid, 700.0, 4);
    simulator.add_order(order);
    order.type = OrderType::Limit;
    order.status = OrderStatus::Add;
    order.amount_rest = order.volume;
    std::vector<Order> _orders = {order};
    data_source->send_tick({0, 0, 70, 3, DealType::Sell});
    EXPECT_EQ(orders, _orders);
}

TEST_F(SimpleMarketSimulatorTest, add_tick_2)
{
    data_source->send_stocks(0,
                std::vector<BQuote>({ BQuote(90, 5, QuoteType::Ask),
                                      BQuote(70, 5, QuoteType::Bid) }));
    Order order(0, 1, OrderDir::Bid, 700.0, 4);
    simulator.add_order(order);
    order.type = OrderType::Limit;
    order.status = OrderStatus::Add;
    order.amount_rest = order.volume;
    std::vector<Order> _orders = {order};
    data_source->send_tick({0, 0, 70, 7, DealType::Sell});
    order.volume = 2;
    order.status = OrderStatus::Fill;
    order.deal_price = 700.0;
    order.amount_rest = 2;
    _orders.push_back(order);
    EXPECT_EQ(orders, _orders);
}

TEST_F(SimpleMarketSimulatorTest, add_tick_3)
{
    data_source->send_stocks(0,
                std::vector<BQuote>({ BQuote(90, 5, QuoteType::Ask),
                                      BQuote(70, 5, QuoteType::Bid) }));
    Order order(0, 1, OrderDir::Bid, 700.0, 4);
    simulator.add_order(order);
    order.type = OrderType::Limit;
    order.status = OrderStatus::Add;
    order.amount_rest = order.volume;
    std::vector<Order> _orders = {order};
    data_source->send_tick({0, 0, 60, 1, DealType::Sell});
    order.volume = 1;
    order.status = OrderStatus::Fill;
    order.deal_price = 700.0;
    order.amount_rest = 3;
    _orders.push_back(order);
    EXPECT_EQ(orders, _orders);
}

TEST_F(SimpleMarketSimulatorTest, add_tick_4)
{
    data_source->send_stocks(0,
                std::vector<BQuote>({ BQuote(90, 5, QuoteType::Ask),
                                      BQuote(70, 5, QuoteType::Bid) }));
    Order order(0, 1, OrderDir::Bid, 700.0, 4);
    Order order2(0, 2, OrderDir::Bid, 700.0, 4);
    simulator.add_order(order);
    simulator.add_order(order2);
    order.type = OrderType::Limit;
    order.status = OrderStatus::Add;
    order.amount_rest = order.volume;
    order2.type = OrderType::Limit;
    order2.status = OrderStatus::Add;
    order2.amount_rest = order.volume;
    std::vector<Order> _orders = {order, order2};
    data_source->send_tick({0, 0, 70, 7, DealType::Sell});
    order.volume = 2;
    order.status = OrderStatus::Fill;
    order.deal_price = 700.0;
    order.amount_rest = 2;
    _orders.push_back(order);
    EXPECT_EQ(orders, _orders);
}

TEST_F(SimpleMarketSimulatorTest, add_tick_5)
{
    data_source->send_stocks(0,
                std::vector<BQuote>({ BQuote(90, 5, QuoteType::Ask),
                                      BQuote(70, 5, QuoteType::Bid) }));
    Order order(0, 1, OrderDir::Bid, 700.0, 4);
    Order order2(0, 2, OrderDir::Bid, 700.0, 4);
    simulator.add_order(order);
    simulator.add_order(order2);
    order.type = OrderType::Limit;
    order.status = OrderStatus::Add;
    order.amount_rest = order.volume;
    order2.type = OrderType::Limit;
    order2.status = OrderStatus::Add;
    order2.amount_rest = order.volume;
    std::vector<Order> _orders = {order, order2};
    data_source->send_tick({0, 0, 70, 10, DealType::Sell});
    order.volume = 4;
    order.status = OrderStatus::Fill;
    order.deal_price = 700.0;
    order.amount_rest = 0;
    _orders.push_back(order);
    order2.volume = 1;
    order2.status = OrderStatus::Fill;
    order2.deal_price = 700.0;
    order2.amount_rest = 3;
    _orders.push_back(order2);
    EXPECT_EQ(orders, _orders);
}

TEST_F(SimpleMarketSimulatorTest, add_tick_6)
{
    data_source->send_stocks(0,
                std::vector<BQuote>({ BQuote(90, 5, QuoteType::Ask),
                                      BQuote(70, 5, QuoteType::Bid) }));
    Order order(0, 1, OrderDir::Bid, 700.0, 4);
    Order order2(0, 2, OrderDir::Bid, 700.0, 4);
    simulator.add_order(order);
    simulator.add_order(order2);
    order.type = OrderType::Limit;
    order.status = OrderStatus::Add;
    order.amount_rest = order.volume;
    order2.type = OrderType::Limit;
    order2.status = OrderStatus::Add;
    order2.amount_rest = order.volume;
    std::vector<Order> _orders = {order, order2};
    data_source->send_tick({0, 0, 60, 5, DealType::Sell});
    order.volume = 4;
    order.status = OrderStatus::Fill;
    order.deal_price = 700.0;
    order.amount_rest = 0;
    _orders.push_back(order);
    order2.volume = 1;
    order2.status = OrderStatus::Fill;
    order2.deal_price = 700.0;
    order2.amount_rest = 3;
    _orders.push_back(order2);
    EXPECT_EQ(orders, _orders);
}

TEST_F(SimpleMarketSimulatorTest, add_tick_7)
{
    data_source->send_stocks(0,
                std::vector<BQuote>({ BQuote(90, 5, QuoteType::Ask),
                                      BQuote(70, 5, QuoteType::Bid) }));
    Order order(0, 1, OrderDir::Bid, 700.0, 4);
    Order order2(0, 2, OrderDir::Bid, 700.0, 4);
    simulator.add_order(order);
    simulator.add_order(order2);
    order.type = OrderType::Limit;
    order.status = OrderStatus::Add;
    order.amount_rest = order.volume;
    order2.type = OrderType::Limit;
    order2.status = OrderStatus::Add;
    order2.amount_rest = order.volume;
    std::vector<Order> _orders = {order, order2};
    data_source->send_tick({0, 0, 70, 6, DealType::Sell});
    order.volume = 1;
    order.status = OrderStatus::Fill;
    order.deal_price = 700.0;
    order.amount_rest = 3;
    _orders.push_back(order);
    data_source->send_tick({0, 0, 70, 5, DealType::Sell});
    order.volume = 3;
    order.status = OrderStatus::Fill;
    order.deal_price = 700.0;
    order.amount_rest = 0;
    _orders.push_back(order);
    order2.volume = 2;
    order2.status = OrderStatus::Fill;
    order2.deal_price = 700.0;
    order2.amount_rest = 2;
    _orders.push_back(order2);
    EXPECT_EQ(orders, _orders);
}
