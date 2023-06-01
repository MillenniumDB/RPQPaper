#pragma once

#include <memory>
#include <queue>
#include <type_traits>

#include "base/binding/binding_id_iter.h"
#include "base/ids/id.h"
#include "base/thread/thread_info.h"
#include "execution/binding_id_iter/paths/any_shortest/search_state.h"
#include "execution/binding_id_iter/paths/index_provider/path_index/path_index.h"
#include "parser/query/paths/automaton/rpq_automaton.h"
#include "third_party/robin_hood/robin_hood.h"

namespace Paths { namespace AnyShortest {

// Dummy structure for template usage
class DummySet {
public:
    static inline void clear() { }
    static inline int end() { return 0; }
    static inline int find(uint64_t) { return 0; }
    static inline void insert(uint64_t) { }
};

/*
BFSEnum returns a single path to all
reachable nodes from a starting node, using BFS.
*/
template <bool MULTIPLE_FINAL>
class BFSEnum : public BindingIdIter {
private:
    // Attributes determined in the constructor
    ThreadInfo*   thread_info;
    VarId         path_var;
    Id            start;
    VarId         end;
    const RPQ_DFA automaton;
    std::unique_ptr<IndexProvider> provider;

    // where the results will be written, determined in begin()
    BindingId* parent_binding;

    // Set of visited SearchStates
    robin_hood::unordered_node_set<SearchState> visited;

    // Queue for BFS. Pointers point to the states in visited
    std::queue<const SearchState*> open;

    // Iterator for index results
    std::unique_ptr<IndexIterator> iter;

    // The index of the transition being currently explored
    uint_fast32_t current_transition;

    // true in the first call of next() and after a reset()
    bool first_next = true;

    // Template type for storing nodes reached with a final state
    typename std::conditional<MULTIPLE_FINAL,
                              robin_hood::unordered_set<uint64_t>,
                              DummySet>::type reached_final;

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


    void analyze(std::ostream& os, int indent = 0) const override;
    void begin(BindingId& parent_binding) override;
    void reset() override;
    bool next() override;

    // Expand neighbors from current state
    const SearchState* expand_neighbors(const SearchState& current_state);

    void assign_nulls() override {
        parent_binding->add(end, ObjectId::get_null());
        parent_binding->add(path_var, ObjectId::get_null());
    }

    // Set iterator for current node + transition
    inline void set_iter(const SearchState& s) {
        // Get current transition object from automaton
        auto& transition = automaton.from_to_connections[s.automaton_state][current_transition];

        // Get iterator from custom index
        iter = provider->get_iterator(transition.type_id.id, transition.inverse, s.node_id.id);
        idx_searches++;
    }
};
}} // namespace Paths::AnyShortest
