#pragma once

#include <memory>
#include <stack>

#include "base/binding/binding_id_iter.h"
#include "base/ids/id.h"
#include "base/thread/thread_info.h"
#include "execution/binding_id_iter/paths/any_simple/search_state.h"
#include "parser/query/paths/automaton/rpq_automaton.h"

namespace Paths { namespace AnySimple {

/*
DFSCheck checks if there's a Simple Path between two fixed nodes, using DFS.
*/
template <bool CYCLIC>
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

    // Stack for search states + paths
    std::stack<SearchStateDFS> open;

    // Statistics
    uint_fast32_t results_found = 0;
    uint_fast32_t idx_searches = 0;

    // Get next state of interest
    SearchStateDFS* expand_neighbors(SearchStateDFS& current_state);

public:
    DFSCheck(ThreadInfo* thread_info,
             VarId       path_var,
             Id          start,
             Id          end,
             RPQ_DFA     automaton,
             std::unique_ptr<IndexProvider> provider);

    void analyze(std::ostream& os, int indent = 0) const override;
    void begin(BindingId& parent_binding) override;
    void reset() override;
    bool next() override;

    // Set iterator for current node + transition
    void set_iter(SearchStateDFS& s) {
        // Get current transition object from automaton
        auto& transition = automaton.from_to_connections[s.automaton_state][s.transition];

        // Get iterator from custom index
        s.iter = provider->get_iterator(transition.type_id.id,
                                        transition.inverse,
                                        s.node_id.id);
        idx_searches++;
    }

    void assign_nulls() override {
        parent_binding->add(path_var, ObjectId::get_null());
    }
};
}} // namespace Paths::AnySimple
