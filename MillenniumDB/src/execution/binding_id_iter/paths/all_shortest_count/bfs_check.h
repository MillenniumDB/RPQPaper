#pragma once

#include <memory>
#include <queue>

#include "base/binding/binding_id_iter.h"
#include "base/ids/id.h"
#include "base/thread/thread_info.h"
#include "execution/binding_id_iter/paths/all_shortest_count/search_state.h"
#include "execution/binding_id_iter/paths/index_provider/path_index/path_index.h"
#include "parser/query/paths/automaton/rpq_automaton.h"
#include "third_party/robin_hood/robin_hood.h"

namespace Paths { namespace AllShortestCount {

/*
AllShortestCount::BFSCheck returns the count of all shortest paths between two fixed nodes.
*/
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

    // Remembers which states were explored. A structure with pointer stability is required
    robin_hood::unordered_node_set<SearchState> visited;

    // Queue for BFS. Pointers point to the states in visited
    std::queue<const SearchState*> open;

    // true in the first call of next() and after a reset()
    bool first_next = true;

    // Optimal distance to target node. UINT64_MAX means the node has not been explored yet.
    uint64_t optimal_distance = UINT64_MAX;

    // Count of shortest paths between the starting and target nodes
    uint64_t path_count = 0;

    // Statistics
    uint_fast32_t results_found = 0;
    uint_fast32_t idx_searches = 0;

public:
    BFSCheck(ThreadInfo* thread_info,
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
        provider    (std::move(provider)) { }

    // Explore all neighbors counting paths reached.
    void explore_neighbors(const SearchState& current_state);

    void analyze(std::ostream& os, int indent = 0) const override;
    void begin(BindingId& parent_binding) override;
    void reset() override;
    bool next() override;

    void assign_nulls() override {
        parent_binding->add(path_var, ObjectId::get_null());
    }
};
}} // namespace Paths::AllShortestCount
