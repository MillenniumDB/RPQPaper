#pragma once

#include <memory>
#include <stack>

#include "base/binding/binding_id_iter.h"
#include "base/ids/id.h"
#include "base/thread/thread_info.h"
#include "execution/binding_id_iter/paths/any/search_state.h"
#include "execution/binding_id_iter/paths/any_shortest/search_state.h"
#include "parser/query/paths/automaton/rpq_automaton.h"
#include "third_party/robin_hood/robin_hood.h"

namespace Paths { namespace Any {

/*
DFSCheck checks if there's a path between two fixed nodes, using DFS.
*/
class DFSCheck : public BindingIdIter {
private:
    // Attributes determined in the constructor
    ThreadInfo*   thread_info;
    VarId         path_var;
    Id            start;
    Id            end;
    const RPQ_DFA automaton;
    std::unique_ptr<IndexProvider> provider;

    // Attributes determined in begin
    BindingId* parent_binding;
    ObjectId end_object_id;
    bool first_next = true;

    // Set of visited SearchStates
    robin_hood::unordered_node_set<Paths::AnyShortest::SearchState> visited;

    // Stack of DFSSearchStates
    std::stack<DFSSearchState> open;

    // Statistics
    uint_fast32_t results_found = 0;
    uint_fast32_t idx_searches  = 0;

public:
    DFSCheck(ThreadInfo* thread_info,
             VarId       path_var,
             Id          start,
             Id          end,
             RPQ_DFA     automaton,
             std::unique_ptr<IndexProvider> provider) :
        thread_info (thread_info),
        path_var    (path_var),
        start       (start),
        end         (end),
        automaton   (automaton),
        provider    (move(provider)) { }

    // Get next state of interest
    robin_hood::unordered_node_set<Paths::AnyShortest::SearchState>::iterator
      expand_neighbors(DFSSearchState& current_state);

    void analyze(std::ostream& os, int indent = 0) const override;
    void begin(BindingId& parent_binding) override;
    void reset() override;
    bool next() override;

    void assign_nulls() override {
        parent_binding->add(path_var, ObjectId::get_null());
    }

    // Set iterator for current node + transition
    inline void set_iter(DFSSearchState& s) {
        // Get current transition object from automaton
        auto& transition = automaton.from_to_connections[s.state][s.current_transition];

        // Get iterator from custom index
        s.iter = provider->get_iterator(transition.type_id.id,
                                        transition.inverse,
                                        s.node_id.id);
        idx_searches++;
    }
};
}} // namespace Paths::Any
