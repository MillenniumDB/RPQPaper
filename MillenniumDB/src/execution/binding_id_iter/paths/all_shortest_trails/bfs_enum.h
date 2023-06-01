#pragma once

#include <memory>
#include <queue>

#include "base/binding/binding_id_iter.h"
#include "base/ids/id.h"
#include "base/thread/thread_info.h"
#include "execution/binding_id_iter/paths/all_shortest_trails/search_state.h"
#include "parser/query/paths/automaton/rpq_automaton.h"
#include "third_party/robin_hood/robin_hood.h"

namespace Paths { namespace AllShortestTrails {

/*
BFSEnum returns all the shortest Trails to all reachable nodes from a starting node.
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

    // Array of all trails (based on prefix tree + linked list idea)
    Visited visited;

    // Queue of search states
    std::queue<SearchState> open;

    // Iterator for index results
    std::unique_ptr<IndexIterator> iter;

    // The index of the transition being currently explored
    uint32_t current_transition;

    bool first_next = true;

    // Map of nodes reached with a final state + their optimal distance: NodeId -> OptimalDistance
    robin_hood::unordered_map<uint64_t, uint32_t> optimal_distances;

    // Statistics
    uint_fast32_t results_found = 0;
    uint_fast32_t idx_searches = 0;

public:
    BFSEnum(ThreadInfo* thread_info,
            VarId       path_var,
            Id          start,
            VarId       end,
            RPQ_DFA     automaton,
            std::unique_ptr<IndexProvider> provider) :
        thread_info (thread_info),
        path_var    (path_var),
        start       (start),
        end         (end),
        automaton   (automaton),
        provider    (move(provider)) { }


    // Expand neighbors from current state
    const SearchState* expand_neighbors(const SearchState& current_state);

    void analyze(std::ostream& os, int indent = 0) const override;
    void begin(BindingId& parent_binding) override;
    void reset() override;
    bool next() override;

    void assign_nulls() override {
        parent_binding->add(end, ObjectId::get_null());
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
}} // namespace Paths::AllShortestTrails