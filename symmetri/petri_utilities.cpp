
#include "petri.h"
namespace symmetri {

size_t toIndex(const std::vector<std::string> &m, const std::string &s) {
  auto ptr = std::find(m.begin(), m.end(), s);
  return std::distance(m.begin(), ptr);
}

bool canFire(const SmallVectorInput &pre,
             const std::vector<AugmentedToken> &tokens) {
  return pre.size() > 0 &&
         std::all_of(pre.begin(), pre.end(), [&](const auto &m_p) {
           size_t actual = std::count(tokens.begin(), tokens.end(), m_p);
           size_t required = std::count(pre.begin(), pre.end(), m_p);
           return actual >= required;
         });
}

gch::small_vector<size_t, 32> possibleTransitions(
    const std::vector<AugmentedToken> &tokens,
    const std::vector<SmallVectorInput> &input_n,
    const std::vector<SmallVector> &p_to_ts_n) {
  gch::small_vector<size_t, 32> possible_transition_list_n;
  for (const AugmentedToken &place : tokens) {
    // transition index
    for (size_t t : p_to_ts_n[std::get<size_t>(place)]) {
      const auto &inputs = input_n[t];
      // current colored place is an input of t
      const bool is_input =
          std::find(inputs.cbegin(), inputs.cend(), place) != inputs.cend();
      // transition is not already considered:
      const bool is_unique = std::find(possible_transition_list_n.cbegin(),
                                       possible_transition_list_n.cend(),
                                       t) == possible_transition_list_n.cend();
      if (is_unique && is_input) {
        possible_transition_list_n.push_back(t);
      }
    }
  }

  return possible_transition_list_n;
}

Reducer createReducerForCallback(const size_t t_i, const Token result) {
  const auto t_end(Clock::now());
  return [=](Petri &model) {
    // if it is in the active transition set it means it is finished and we
    // should process it.
    const auto it = std::find(model.scheduled_callbacks.cbegin(),
                              model.scheduled_callbacks.cend(), t_i);
    if (it != model.scheduled_callbacks.cend()) {
      const auto &place_list = model.net.output_n[t_i];
      model.tokens.reserve(model.tokens.size() + place_list.size());
      for (const auto &[p, c] : place_list) {
        model.tokens.push_back({p, result});
      }
      model.scheduled_callbacks.erase(it);
    };
    model.log.push_back({t_i, result, t_end});
  };
}

Reducer scheduleCallback(
    size_t t_i, const Callback &task,
    const std::shared_ptr<moodycamel::BlockingConcurrentQueue<Reducer>>
        &reducer_queue) {
  reducer_queue->enqueue([start_time = Clock::now(), t_i](Petri &model) {
    model.log.push_back({t_i, Started, start_time});
  });

  return createReducerForCallback(t_i, fire(task));
}

}  // namespace symmetri
