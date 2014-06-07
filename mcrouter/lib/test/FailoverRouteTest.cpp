#include <memory>
#include <vector>

#include <gtest/gtest.h>

#include "mcrouter/lib/McReply.h"
#include "mcrouter/lib/McRequest.h"
#include "mcrouter/lib/routes/FailoverRoute.h"
#include "mcrouter/lib/test/RouteHandleTestUtil.h"

using namespace facebook::memcache;

using std::make_shared;
using std::vector;

TEST(failoverRouteTest, success) {
  vector<std::shared_ptr<TestHandle>> test_handles{
    make_shared<TestHandle>(GetRouteTestData(mc_res_found, "a")),
    make_shared<TestHandle>(GetRouteTestData(mc_res_found, "b")),
    make_shared<TestHandle>(GetRouteTestData(mc_res_found, "c"))
  };

  TestRouteHandle<FailoverRoute<TestRouteHandleIf>> rh(
    get_route_handles(test_handles));

  auto reply = rh.route(McRequest("0"), McOperation<mc_op_get>());
  EXPECT_TRUE(reply.value().dataRange().str() == "a");
}

TEST(failoverRouteTest, once) {
  vector<std::shared_ptr<TestHandle>> test_handles{
    make_shared<TestHandle>(GetRouteTestData(mc_res_timeout, "a")),
    make_shared<TestHandle>(GetRouteTestData(mc_res_found, "b")),
    make_shared<TestHandle>(GetRouteTestData(mc_res_found, "c"))
  };

  TestRouteHandle<FailoverRoute<TestRouteHandleIf>> rh(
    get_route_handles(test_handles));

  auto reply = rh.route(McRequest("0"), McOperation<mc_op_get>());
  EXPECT_TRUE(reply.value().dataRange().str() == "b");
}

TEST(failoverRouteTest, twice) {
  vector<std::shared_ptr<TestHandle>> test_handles{
    make_shared<TestHandle>(GetRouteTestData(mc_res_timeout, "a")),
    make_shared<TestHandle>(GetRouteTestData(mc_res_timeout, "b")),
    make_shared<TestHandle>(GetRouteTestData(mc_res_found, "c"))
  };

  TestRouteHandle<FailoverRoute<TestRouteHandleIf>> rh(
    get_route_handles(test_handles));

  auto reply = rh.route(McRequest("0"), McOperation<mc_op_get>());
  EXPECT_TRUE(reply.value().dataRange().str() == "c");
}

TEST(failoverRouteTest, fail) {
  vector<std::shared_ptr<TestHandle>> test_handles{
    make_shared<TestHandle>(GetRouteTestData(mc_res_timeout, "a")),
    make_shared<TestHandle>(GetRouteTestData(mc_res_timeout, "b")),
    make_shared<TestHandle>(GetRouteTestData(mc_res_timeout, "c"))
  };

  TestRouteHandle<FailoverRoute<TestRouteHandleIf>> rh(
    get_route_handles(test_handles));

  auto reply = rh.route(McRequest("0"), McOperation<mc_op_get>());

  /* Will return the last reply when ran out of targets */
  EXPECT_EQ(reply.value().dataRange().str(), "c");
}