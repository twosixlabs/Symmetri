#include <iostream>

#include "doctest/doctest.h"
#include "petri.h"
#include "symmetri/utilities.hpp"
using namespace symmetri;

CREATE_CUSTOM_TOKEN(Blue)
CREATE_CUSTOM_TOKEN(Red)

Token red() { return Red; }

TEST_CASE("Test color.") {
  Net net = {{{"t0"}, {{{"p1", Blue}}, {{"p2", Red}}}}};
  PriorityTable priority;
  Marking m0 = {{"p1", Blue}};
  Marking mgoal = {{"p1", Red}};
  auto threadpool = std::make_shared<TaskSystem>(1);
  Petri m(net, priority, m0, mgoal, "s", threadpool);

  m.net.registerCallback("t0", &red);

  for (auto [p, c] : m.getMarking()) {
    std::cout << p << ", " << c.toIndex() << std::endl;
  }
  // t0 is enabled.
  m.fireTransitions();
  Reducer r1;
  CHECK(m.reducer_queue->wait_dequeue_timed(r1, std::chrono::seconds(1)));
  r1(m);
  CHECK(m.reducer_queue->wait_dequeue_timed(r1, std::chrono::seconds(1)));
  r1(m);

  const auto marking = m.getMarking();
  // processed but post are not:
  {
    Marking expected = {{"p2", Red}};
    for (auto [p, c] : marking) {
      std::cout << p << ", " << c.toIndex() << std::endl;
    }
    CHECK(MarkingEquality(marking, expected));
  }
}
