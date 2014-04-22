#include "utils/test_main.hxx"

#include "nmranet/EventHandlerTemplates.hxx"
#include "nmranet/NMRAnetEventTestHelper.hxx"

using ::testing::_;
using ::testing::Mock;
using ::testing::StrictMock;

namespace NMRAnet {

class SimpleEventProxy : public ProxyEventHandler {
 public:
  SimpleEventProxy(NMRAnetEventHandler* handler)
      : handler_(handler) {}

  virtual void HandlerFn(EventHandlerFunction fn,
                         EventReport* event,
                         BarrierNotifiable* done) {
    (handler_->*fn)(event, done);
  }

 private:
  NMRAnetEventHandler* handler_;
};

TEST(EventProxy, TestHandleEvent) {
  StrictMock<MockEventHandler> inner_handler;
  SimpleEventProxy proxy(&inner_handler);
  EXPECT_CALL(inner_handler, HandleEventReport(_, _)).Times(1);
  proxy.HandleEventReport(NULL, NULL);
}

TEST(EventProxy, ConsumerRange) {
  StrictMock<MockEventHandler> inner_handler;
  SimpleEventProxy proxy(&inner_handler);
  EXPECT_CALL(inner_handler, HandleConsumerRangeIdentified(_, _)).Times(1);
  proxy.HandleConsumerRangeIdentified(NULL, NULL);
}

}  // namespace NMRAnet
