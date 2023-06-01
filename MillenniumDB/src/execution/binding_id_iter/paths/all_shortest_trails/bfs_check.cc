#include "bfs_check.h"

#include "base/ids/var_id.h"
#include "execution/binding_id_iter/paths/path_manager.h"

using namespace std;
using namespace Paths::AllShortestTrails;


void BFSCheck::begin(BindingId& _parent_binding) {
    parent_binding = &_parent_binding;

    // setted at object initialization:
    // first_next = true;
    // optimal_distance = UINT64_MAX;

    // Add starting states to open and visited
    ObjectId start_object_id(std::holds_alternative<ObjectId>(start) ? std::get<ObjectId>(start)
                                                                     : (*parent_binding)[std::get<VarId>(start)]);
    auto start_node_visited = visited.add(start_object_id, ObjectId(), ObjectId(), false, nullptr);
    open.emplace(start_node_visited, automaton.start_state, 0);

    // Store ID for end object
    end_object_id = std::holds_alternative<ObjectId>(end) ? std::get<ObjectId>(end)
                                                          : (*parent_binding)[std::get<VarId>(end)];
    iter = make_unique<NullIndexIterator>();
}


void BFSCheck::reset() {
    // clear bfs structures
    queue<SearchState> empty;
    open.swap(empty);
    visited.clear();

    first_next = true;
    optimal_distance = UINT64_MAX;

    // Add starting states to open and visited
    ObjectId start_object_id(std::holds_alternative<ObjectId>(start) ? std::get<ObjectId>(start)
                                                                     : (*parent_binding)[std::get<VarId>(start)]);
    auto start_node_visited = visited.add(start_object_id, ObjectId(), ObjectId(), false, nullptr);
    open.emplace(start_node_visited, automaton.start_state, 0);

    // Store ID for end object
    end_object_id = std::holds_alternative<ObjectId>(end) ? std::get<ObjectId>(end)
                                                          : (*parent_binding)[std::get<VarId>(end)];
    iter = make_unique<NullIndexIterator>();
}


bool BFSCheck::next() {
    // Check if first state is final
    if (first_next) {
        first_next = false;
        auto& current_state = open.front();

        // Return false if node does not exist in the database
        if (!provider->node_exists(current_state.path_state->node_id.id)) {
            open.pop();
            return false;
        }

        // Starting state is solution
        if (automaton.is_final_state[automaton.start_state] && current_state.path_state->node_id == end_object_id) {
            auto path_id = path_manager.set_path(current_state.path_state, path_var);
            parent_binding->add(path_var, path_id);
            results_found++;
            queue<SearchState> empty;  // Empty queue because the only state with 0 distance is the first state
            open.swap(empty);
            return true;
        }
    }

    while (open.size() > 0) {
        auto& current_state = open.front();
        auto reached_final_state = expand_neighbors(current_state);

        // Enumerate reached solutions
        if (reached_final_state != nullptr) {
            auto path_id = path_manager.set_path(reached_final_state, path_var);
            parent_binding->add(path_var, path_id);
            results_found++;
            return true;
        } else {
            open.pop(); // Pop and visit next state
        }
    }
    return false;
}


const PathState* BFSCheck::expand_neighbors(const SearchState& current_state) {
    // Check if this is the first time that current_state is explored
    if (iter->at_end()) {
        current_transition = 0;
        // Check if automaton state has transitions
        if (automaton.from_to_connections[current_state.automaton_state].size() == 0) {
            return nullptr;
        }
        set_iter(current_state);
    }

    // Iterate over the remaining transitions of current_state
    // Don't start from the beginning, resume where it left thanks to current_transition and iter (pipeline)
    while (current_transition < automaton.from_to_connections[current_state.automaton_state].size()) {
        auto& transition = automaton.from_to_connections[current_state.automaton_state][current_transition];

        // Iterate over records until a final state is reached
        while (iter->next()) {
            // Reconstruct path and check if it's a trail, discard paths that are not trails
            if (!is_trail(current_state.path_state, ObjectId(iter->get_edge()))) {
                continue;
            }

            // Special Case: A solution has been reached (don't expand)
            if (automaton.is_final_state[transition.to] && ObjectId(iter->get_node()) == end_object_id) {
                if (optimal_distance != UINT64_MAX) {  // Only return shortest paths
                    if (optimal_distance == current_state.distance + 1) {
                        return visited.add(ObjectId(iter->get_node()),
                                           ObjectId(iter->get_edge()),
                                           transition.type_id,
                                           transition.inverse,
                                           current_state.path_state);
                    }  // Else: continue
                } else {
                    optimal_distance = current_state.distance + 1;
                    return visited.add(ObjectId(iter->get_node()),
                                       ObjectId(iter->get_edge()),
                                       transition.type_id,
                                       transition.inverse,
                                       current_state.path_state);
                }  // Else: continue
                continue;
            }

            // Add new path state to visited
            auto new_visited_ptr = visited.add(ObjectId(iter->get_node()),
                                               ObjectId(iter->get_edge()),
                                               transition.type_id,
                                               transition.inverse,
                                               current_state.path_state);

            // Add new state to open if it's a potential optimal path
            // When the first optimal solution is reached, all other paths to potential solutions are already in open
            if (optimal_distance == UINT64_MAX) {
                open.emplace(new_visited_ptr, transition.to, current_state.distance + 1);
            }
        }

        // Construct new iter with the next transition (if there exists one)
        current_transition++;
        if (current_transition < automaton.from_to_connections[current_state.automaton_state].size()) {
            set_iter(current_state);
        }
    }
    return nullptr;
}


void BFSCheck::analyze(std::ostream& os, int indent) const {
    os << std::string(indent, ' ');
    os << "Paths::AllShortestTrails::BFSCheck(idx_searches: " << idx_searches << ", found: " << results_found << ")";
}