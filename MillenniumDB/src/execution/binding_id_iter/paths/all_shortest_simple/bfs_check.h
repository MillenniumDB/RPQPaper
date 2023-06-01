#pragma once

#include <memory>
#include <queue>

#include "base/binding/binding_id_iter.h"
#include "base/ids/id.h"
#include "base/thread/thread_info.h"
#include "execution/binding_id_iter/paths/all_shortest_simple/search_state.h"
#include "parser/query/paths/automaton/rpq_automaton.h"

namespace Paths { namespace AllShortestSimple {

/*
AllShortestSimple::BFSCheck returns all the simple paths between two fixed nodes.
If CYCLIC template is true, the path may have the same node for the start and end node
*/
template <bool CYCLIC>
class BFSCheck : public BindingIdIter {
private:
    // Attributes determined in the constructor
    ThreadInfo*   thread_info;
    VarId         path_var;
    Id            start;
    Id            end;
    const RPQ_DFA automaton;
    std::unique_ptr<IndexProvider> provider;

    // where the results will be written, determined in begin()
    BindingId* parent_binding;

    // if `end` is an ObjectId, this has the same value
    // if `end` is a variable, this has its the value in the binding
    // its value is setted in begin() and reset()
    ObjectId end_object_id;

    // Array of all simple paths (based on prefix tree + linked list idea)
    Visited visited;

    // Queue for BFS
    std::queue<SearchState> open;

    // Iterator for index results
    std::unique_ptr<IndexIterator> iter;

    // The index of the transition being currently explored
    uint_fast32_t current_transition;

    // true in the first call of next() and after a reset()
    bool first_next = true;

    // Optimal distance to target node. UINT64_MAX means the node has not been explored yet.
    uint64_t optimal_distance = UINT64_MAX;

    // Statistics
    uint_fast32_t results_found = 0;
    uint_fast32_t idx_searches = 0;

public:
    BFSCheck(ThreadInfo* thread_info,
             VarId       path_var,
             Id          start,
             Id          end,
             RPQ_DFA     automaton,
             std::unique_ptr<IndexProvider> provider);

    // Explore neighbors searching for a solution.
    // returns a pointer to the object added to visited when a solution is found
    // or nullptr when there are no more results
    const PathState* expand_neighbors(const SearchState& current_state);

    void analyze(std::ostream& os, int indent = 0) const override;

    void begin(BindingId& parent_binding) override;

    void reset() override;

    bool next() override;

    void assign_nulls() override {
        parent_binding->add(path_var, ObjectId::get_null());
    }

    // Set iterator for current node + transition
    inline void set_iter(const SearchState& s) {
        // Get current transition object from automaton
        auto& transition = automaton.from_to_connections[s.automaton_state][current_transition];

        // Get iterator from custom index
        iter = provider->get_iterator(transition.type_id.id,
                                      transition.inverse,
                                      s.path_state->node_id.id);
        idx_searches++;
    }
};
}} // namespace Paths::AllShortestSimple
