#include "bfs_check.h"

#include "execution/binding_id_iter/paths/path_manager.h"

using namespace std;
using namespace Paths::AllShortestCount;

void BFSCheck::begin(BindingId& _parent_binding) {
    parent_binding = &_parent_binding;

    // setted at object initialization:
    // first_next = true;
    // optimal_distance = UINT64_MAX;
    // path_count = 0;

    // Add starting states to open and visited
    ObjectId start_object_id(std::holds_alternative<ObjectId>(start) ? std::get<ObjectId>(start)
                                                                     : (*parent_binding)[std::get<VarId>(start)]);
    auto start_state = visited.emplace(start_object_id, automaton.start_state, 0, 1);
    open.push(start_state.first.operator->());

    // Store ID for end object
    end_object_id = std::holds_alternative<ObjectId>(end) ? std::get<ObjectId>(end)
                                                          : (*parent_binding)[std::get<VarId>(end)];
}


void BFSCheck::reset() {
    // Empty open and visited
    queue<const SearchState*> empty;
    open.swap(empty);
    visited.clear();

    first_next = true;
    optimal_distance = UINT64_MAX;
    path_count = 0;

    // Add starting state to open and visited
    ObjectId start_object_id(std::holds_alternative<ObjectId>(start) ? std::get<ObjectId>(start)
                                                                     : (*parent_binding)[std::get<VarId>(start)]);
    auto start_state = visited.emplace(start_object_id, automaton.start_state, 0, 1);
    open.push(start_state.first.operator->());

    // Store ID for end object
    end_object_id = std::holds_alternative<ObjectId>(end) ? std::get<ObjectId>(end)
                                                          : (*parent_binding)[std::get<VarId>(end)];
}


bool BFSCheck::next() {
    // Check if first state is final
    if (first_next) {
        first_next = false;
        auto current_state = open.front();

        // Return false if node does not exist in the database
        if (!provider->node_exists(current_state->node_id.id)) {
            open.pop();
            return false;
        }

        // Starting state is solution
        if (automaton.is_final_state[automaton.start_state] && current_state->node_id == end_object_id) {
            path_count += current_state->count;
            parent_binding->add(path_var, ObjectId(ObjectId::MASK_POSITIVE_INT | path_count));

            // Empty queue because the only state with 0 distance is the first state
            open.pop();
            results_found = path_count;
            return true;
        }
    }

    while (open.size() > 0) {
        auto current_state = open.front();
        explore_neighbors(*current_state);

        // Check if current paths are solution
        if (automaton.is_final_state[current_state->automaton_state] && current_state->node_id == end_object_id) {
            parent_binding->add(path_var, ObjectId(ObjectId::MASK_POSITIVE_INT | path_count));

            // Empty queue, no more optimal paths can be counted
            queue<const SearchState*> empty;
            open.swap(empty);
            results_found = path_count;
            return true;
        } else {
            open.pop();
        }
    }
    return false;
}


void BFSCheck::explore_neighbors(const SearchState& current_state) {
    for (size_t current_transition = 0;
         current_transition < automaton.from_to_connections[current_state.automaton_state].size();
         current_transition++)
    {
        auto& transition = automaton.from_to_connections[current_state.automaton_state][current_transition];

        // Get iterator from custom index
        auto iter = provider->get_iterator(transition.type_id.id, transition.inverse, current_state.node_id.id);
        idx_searches++;

        // Iterate over records while counting paths, until all neighbors of the current state are visited
        while (iter->next()) {
            SearchState next_state(ObjectId(iter->get_node()),
                                   transition.to,
                                   current_state.distance + 1,
                                   current_state.count);

            // Check if next state has already been visited
            auto visited_search = visited.find(next_state);
            if (visited_search != visited.end()) {
                // Consider next_state only if it has an optimal distance
                if (visited_search->distance == next_state.distance) {
                    visited_search->count += current_state.count;

                    // Check if current paths are solution, and add them if true
                    if (automaton.is_final_state[next_state.automaton_state] && next_state.node_id == end_object_id) {
                        if (optimal_distance == next_state.distance) {
                            path_count += next_state.count;
                        }
                    }
                }
            } else {
                // Add state to visited and open
                auto new_visited_state = visited.emplace(ObjectId(iter->get_node()),
                                                         transition.to,
                                                         current_state.distance + 1,
                                                         current_state.count).first;
                open.push(new_visited_state.operator->());

                // Check if current paths are solution, and add them if true
                if (automaton.is_final_state[transition.to] && next_state.node_id == end_object_id) {
                    if (optimal_distance != UINT64_MAX) {
                        if (optimal_distance == next_state.distance) {
                            path_count += next_state.count;
                        }
                    } else {
                        optimal_distance = next_state.distance;
                        path_count = next_state.count;
                    }
                }
            }
        }
    }
}


void BFSCheck::analyze(std::ostream& os, int indent) const {
    os << std::string(indent, ' ');
    os << "Paths::AllShortestCount::BFSCheck(idx_searches: " << idx_searches << ", found: " << results_found << ")";
}
