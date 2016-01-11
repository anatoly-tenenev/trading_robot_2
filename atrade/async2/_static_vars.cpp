#include "add_order.hpp"
#include "market_service.hpp"
#include "timer.hpp"
#include "req_securities.hpp"
#include "wait_deal.hpp"
#include "cancel_order.hpp"
#include "move_order.hpp"

namespace atrade {
namespace async2 {

asio::io_service::id AddOrderService::id;

asio::io_service::id MarketService::id;

asio::io_service::id TimerService::id;

asio::io_service::id ReqSecuritiesService::id;

asio::io_service::id WaitDealService::id;

asio::io_service::id CancelOrderService::id;

asio::io_service::id MoveOrderService::id;

}
}
