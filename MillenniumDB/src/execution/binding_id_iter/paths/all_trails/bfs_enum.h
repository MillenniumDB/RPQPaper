#pragma once

#include <memory>
#include <queue>

#include "base/binding/binding_id_iter.h"
#include "base/ids/id.h"
#include "base/thread/thread_info.h"
#include "execution/binding_id_iter/paths/all_trails/search_state.h"
#include "parser/query/paths/automaton/rpq_automaton.h"

namespace Paths { namespace AllTrails {

/*
BFSEnum returns all Trails to all
reachable nodes from a starting node, using BFS.
*/
class BFSEnum : public BindingIdIter {
private:
    // Attributes determined in the constructor
    ThreadInfo*   thread_info;
    VarId         path_var;
    Id            start;
    VarId         end;
    const RPQ_DFA automaton;
    std::unique_ptr<IndexProvider> provider;

    // Attributes determined in begin
    BindingId* parent_binding;
    bool first_next = true;

    // Array of all trails (based on prefix tree + linked list idea)
    Visited visited;

    // Queue of search states
    std::queue<SearchState> open;

    // The index of the transition being currently explored
    uint32_t current_transition = 0;

    // Statistics
    uint_fast32_t results_found = 0;
    uint_fast32_t idx_searches = 0;

    // Expand neighbors from current state
    const SearchState* expand_neighbors(const SearchState& current_state);

    // Iterator for index results
    std::unique_ptr<IndexIterator> iter;

public:
    BFSEnum(ThreadInfo* thread_info,
            VarId       path_var,
            Id          start,
            VarId       end,
            RPQ_DFA     automaton,
            std::unique_ptr<IndexProvider> provider)  :
        thread_info (thread_info),
        path_var    (path_var),
        start       (start),
        end         (end),
        automaton   (automaton),
        provider    (move(provider)) { }

    void analyze(std::ostream& os, int indent = 0) const override;
    void begin(BindingId& parent_binding) override;
    void reset() override;

    bool next() override;

    void assign_nulls() override {
        parent_binding->add(end, ObjectId::get_null());
    }

    // Set iterator for current node + transition
    void set_iter(const SearchState& s) {
        // Get current transition object from automaton
        auto& transition = automaton.from_to_connections[s.automaton_state][current_transition];

        // Get iterator from custom index
        iter = provider->get_iterator(transition.type_id.id,
                                      transition.inverse,
                                      s.path_state->node_id.id);
        idx_searches++;
    }
};
}} // namespace Paths::AllTrails
