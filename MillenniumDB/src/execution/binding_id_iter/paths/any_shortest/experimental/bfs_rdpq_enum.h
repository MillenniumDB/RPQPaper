#pragma once

#include <array>
#include <memory>
#include <queue>
#include <variant>

#include "base/binding/binding_id_iter.h"
#include "base/thread/thread_info.h"
#include "execution/binding_id_iter/paths/any_shortest/search_state.h"
#include "execution/binding_id_iter/scan_ranges/scan_range.h"
#include "parser/query/paths/automaton/rdpq_automaton.h"
#include "storage/index/bplus_tree/bplus_tree.h"
#include "third_party/robin_hood/robin_hood.h"

namespace Paths { namespace AnyShortest {

/*
BFS_RDPQEnum enumerates paths 'from' or 'to' a specific node, using the BFS algorithm and the DE automaton.
It uses class members to keep track of the current edge & data transitions that are being explored.
*/
class BFS_RDPQEnum : public BindingIdIter {
private:
    // Attributes determined in the constructor
    ThreadInfo*   thread_info;
    VarId         path_var;
    Id            start;
    VarId         end;
    RDPQAutomaton automaton;

    // Attributes determined in begin
    BindingId* parent_binding;
    bool first_next = true;

    // Ranges to search in BPT. They are not local variables because some positions are reused.
    std::array<uint64_t, 4> min_ids;
    std::array<uint64_t, 4> max_ids;

    robin_hood::unordered_node_set<SearchState> visited;
    // open stores a pointer to a SearchState stored in visited
    // that allows to avoid use visited.find to get a pointer and
    // use the state extracted of open directly.
    std::queue<const SearchState*> open;

    // Stores the children of state in expansion
    std::unique_ptr<BptIter<4>> iter;
    std::unique_ptr<Record<4UL>> child_record;

    // The index of the transitions that are currently being explored
    uint32_t current_data_transition;
    uint32_t current_edge_transition;

    // Statistics
    uint_fast32_t results_found = 0;
    uint_fast32_t bpt_searches = 0;

public:
    BFS_RDPQEnum(ThreadInfo*  thread_info,
                VarId         path_var,
                Id            start,
                VarId         end,
                RDPQAutomaton automaton);

    // Evaluate data checks for a specific node
    bool eval_data_check(uint64_t node, std::vector<std::tuple<Operators, std::string, QueryElement>>& property_checks);

    // Obtain the next state to add to open (if it exists)
    robin_hood::unordered_node_set<SearchState>::iterator
      expand_neighbors(const SearchState& current_state);

    void analyze(std::ostream& os, int indent = 0) const override;
    void begin(BindingId& parent_binding) override;
    void reset() override;
    bool next() override;

    void assign_nulls() override {
        parent_binding->add(end, ObjectId::get_null());
        parent_binding->add(path_var, ObjectId::get_null());
    }

    // Set iter attribute that obtains all states that are connected with
    // the current_state through a specific edge transition
    void set_iter(const SearchState& current_state);
};
}} // namespace Paths::AnyShortest
