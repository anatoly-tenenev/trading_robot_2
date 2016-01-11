#ifndef ATRADE_HEADERS_H
#define ATRADE_HEADERS_H

#include <vector>
#include <functional>
#include <queue>
#include <memory>
#include <unordered_map>
#include <map>
#include <algorithm>
#include <iterator>
#include <exception>
#include <unordered_set>
#include <set>
#include <fstream>

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <boost/asio/io_service.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/asio/deadline_timer.hpp>

#include <boost/filesystem.hpp>

#include <spdlog/spdlog.h>

namespace atrade
{
    namespace pt = boost::posix_time;
    namespace gr = boost::gregorian;
    namespace asio = boost::asio;
    namespace system = boost::system;
    namespace filesystem = boost::filesystem;

    template <typename Key, typename Value>
    using umap = std::unordered_map<Key, Value>;

    template <typename Key, typename Signature>
    using fumap = umap<Key, std::function<Signature>>;

    template <typename Value>
    using uset = std::unordered_set<Value>;

    template <typename T>
    using vector_uptr = std::vector<std::unique_ptr<T>>;

    template <typename T>
    using vector_sptr = std::vector<std::shared_ptr<T>>;

    class IMarket;
    class Security;
    class Lock;

    namespace backtester
    {
        class IMarketSimulator;
        class MarketSimulator;

    }

    namespace async2
    {
        class IAsyncMarket;
        class AsyncMarketBase;
        class AsyncMarket;
        class RootAsyncMarket;
        template <typename THandler> class Subscription;
        template <typename THandler> class SubscriptionService;
        class History;
        class Task;
        class TaskService;
        class TradingContext;
        class IStrategy;
    }

}

#endif // ATRADE_HEADERS_H
