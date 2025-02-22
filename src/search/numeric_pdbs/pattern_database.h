#ifndef NUMERIC_PDBS_NUMERIC_PATTERN_DATABASE_H
#define NUMERIC_PDBS_NUMERIC_PATTERN_DATABASE_H

#include "numeric_condition.h"
#include "numeric_state_registry.h"
#include "types.h"

#include "../task_proxy.h" // TODO get rid of this

#include <utility>
#include <vector>


namespace numeric_pdb_helper {
class NumericOperatorProxy;
class NumericTaskProxy;
}

namespace numeric_pdbs {

class AbstractOperator {
    /*
      This class represents an abstract operator how it is needed for
      the search performed during the PDB-construction. As
      all abstract states are represented as a number, abstract
      operators don't have "usual" effects but "hash effects", i.e. the
      change (as number) the abstract operator implies on a given
      abstract state.
    */

    int op_id;

    ap_float cost;

    /*
      Propositional preconditions for the search.
    */
    std::vector<std::pair<int, int>> preconditions;

    /*
      Propositional effect of the operator during search on a given abstract state number.
    */
    std::size_t hash_effect;
public:
    /*
      Abstract operators are built from concrete operators. The
      parameters follow the usual name convention of SAS+ operators,
      meaning prevail, preconditions and effects are all related to
      progression search.
    */
    AbstractOperator(const std::vector<std::pair<int, int>> &prevail,
                     const std::vector<std::pair<int, int>> &preconditions,
                     const std::vector<std::pair<int, int>> &effects,
                     int op_id,
                     ap_float cost,
                     const std::vector<std::size_t> &hash_multipliers,
                     bool regression);

    /*
      Returns variable value pairs which represent the preconditions of
      the abstract operator in a search
    */
    const std::vector<std::pair<int, int>> &get_preconditions() const {
        return preconditions;
    }

    /*
      Returns the effect of the abstract operator in form of a value
      change (+ or -) to an abstract state index
    */
    std::size_t get_hash_effect() const {return hash_effect; }

    /*
      Returns the cost of the abstract operator (same as the cost of
      the original concrete operator)
    */
    ap_float get_cost() const {
        return cost;
    }

    int get_op_id() const {
        return op_id;
    }

    void dump(const Pattern &pattern,
              const numeric_pdb_helper::NumericTaskProxy &task_proxy) const;
};

// Implements a single pattern database
class PatternDatabase {
    std::shared_ptr<numeric_pdb_helper::NumericTaskProxy> task_proxy;

    Pattern pattern;

    std::unique_ptr<NumericStateRegistry> state_registry;

    // final h-values for abstract-states
    std::vector<ap_float> distances;

    // multipliers for each propositional variable for perfect hash function
    std::vector<std::size_t> prop_hash_multipliers;

    std::vector<std::pair<int, int>> propositional_goals;
    std::vector<numeric_condition::RegularNumericCondition> numeric_goals;

    ap_float min_action_cost;

    bool exhausted_abstract_state_space;

    mutable std::vector<ap_float> tmp_abstract_numeric_state; // avoid reallocation

    /*
      Recursive method; called by build_abstract_operators. In the case
      of a precondition with value = -1 in the concrete operator, all
      multiplied out abstract operators are computed, i.e. for all
      possible values of the variable (with precondition = -1), one
      abstract operator with a concrete value (!= -1) is computed.
    */
    void multiply_out(
        int pos, int op_id, ap_float cost,
        std::vector<std::pair<int, int>> &prev_pairs,
        std::vector<std::pair<int, int>> &pre_pairs,
        std::vector<std::pair<int, int>> &eff_pairs,
        const std::vector<std::pair<int, int>> &effects_without_pre,
        std::vector<AbstractOperator> &operators,
        bool regression);

    /*
      Computes all abstract operators for a given concrete operator (by
      its global operator number). Initializes data structures for initial
      call to recursive method multiply_out. variable_to_index maps
      variables in the task to their index in the pattern or -1.
    */
    void build_abstract_operators(
        const numeric_pdb_helper::NumericOperatorProxy &op,
        ap_float cost,
        const std::vector<int> &variable_to_index,
        std::vector<AbstractOperator> &operators,
        bool regression);

    bool is_applicable(const NumericState &state,
                       const numeric_pdb_helper::NumericOperatorProxy &op,
                       const std::vector<int> &num_variable_to_index) const;

    std::vector<ap_float> get_numeric_successor(std::vector<ap_float> state,
                                                const numeric_pdb_helper::NumericOperatorProxy &op,
                                                const std::vector<int> &num_variable_to_index) const;

    void build_goals(const std::vector<int> &variable_to_index,
                     const std::vector<int> &num_variable_to_index);

    /*
      Computes all abstract operators, builds the match tree (successor
      generator) and then does a Uniform Cost Search to compute
      all final h-values (stored in distances). operator_costs can
      specify individual operator costs for each operator for action
      cost partitioning. If left empty, default operator costs are used.
    */
    void create_pdb(
            std::size_t max_number_states,
            const std::vector<ap_float> &operator_costs = std::vector<ap_float>(),
            bool dump = false);

    void create_pdb_propositional(
            size_t number_states,
            const std::vector<ap_float> &operator_costs = std::vector<ap_float>());

    /*
      Sets the pattern for the PDB and initializes prop_hash_multipliers and
      num_states. operator_costs can specify individual operator costs
      for each operator for action cost partitioning. If left empty,
      default operator costs are used.
    */
    void set_pattern(
            const Pattern &pattern,
            const std::vector<ap_float> &operator_costs = std::vector<ap_float>());

    /*
      For a given abstract state (given as index), the according values
      for each variable in the state are computed and compared with the
      given pairs of goal variables and values. Returns true iff the
      state is a goal state.
    */
    bool is_goal_state(
            const NumericState &state,
            const std::vector<int> &num_variable_to_index) const;

    bool is_abstract_goal_state(const State &state) const;

    /*
      The given concrete state is used to calculate the index of the
      according abstract state. This is only used for table lookup
      (distances) during search.
    */
    std::size_t prop_hash_index(const State &state) const;

    const std::vector<ap_float> &get_abstract_numeric_state(const State &state) const;

public:
    /*
      Important: It is assumed that the pattern (passed via Options) is
      sorted, contains no duplicates and is small enough so that the
      number of abstract states is below numeric_limits<int>::max()
      Parameters:
       dump:           If set to true, prints the construction time.
       operator_costs: Can specify individual operator costs for each
       operator. This is useful for action cost partitioning. If left
       empty, default operator costs are used.
    */
    PatternDatabase(
            const std::shared_ptr<numeric_pdb_helper::NumericTaskProxy> task_proxy,
            const Pattern &pattern,
            std::size_t max_number_states,
            bool dump = false,
            const std::vector<ap_float> &operator_costs = std::vector<ap_float>());

    ~PatternDatabase() = default;

    std::pair<bool, ap_float> get_value(const State &state) const;

    // Returns the pattern (i.e. all variables used) of the PDB
    const Pattern &get_pattern() const {
        return pattern;
    }

    // Returns the size (number of abstract states) of the PDB
    std::size_t get_size() const {
        return distances.size();
    }

    /*
      Returns the average h-value over all states, where dead-ends are
      ignored (they neither increase the sum of all h-values nor the
      number of entries for the mean value calculation). If all states
      are dead-ends, infinity is returned.
      Note: This is only calculated when called; avoid repeated calls to
      this method!
    */
    ap_float compute_mean_finite_h() const;

    // Returns true iff op has an effect on a variable in the pattern.
    bool is_operator_relevant(const OperatorProxy &op) const;
};
}

#endif
