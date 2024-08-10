#ifndef NUMERIC_PDBS_NUMERIC_HELPER
#define NUMERIC_PDBS_NUMERIC_HELPER

#include "numeric_condition.h"

#include "../option_parser.h"
#include "../plugin.h"
#include "../task_tools.h"

#include <cmath>
#include <iostream>
#include <list>
#include <set>
#include <unordered_map>
#include <vector>

namespace arithmetic_expression {
class ArithmeticExpression;
}

namespace numeric_pdb_helper {

/* An action is an operator where effects are espressed as add and eff of
 proposition. A proposition is an atom of the form Var = Val */

struct Action {
    std::vector<ap_float> eff_list;  // simple numeric effects
    // list of assignment effects: first is global numeric var id, second is assigned value
    std::vector<std::pair<int, ap_float>> asgn_effs;

    explicit Action(int size_eff) : eff_list(size_eff, 0) {
    }
};

/* Linear Numeric Conditions */

struct LinearNumericCondition {
    std::vector<ap_float> coefficients;
    ap_float constant;

    explicit LinearNumericCondition(int size_coefficients) {
        coefficients.assign(size_coefficients, 0);
        constant = 0;
    }
};

std::ostream &operator<<(std::ostream &os, const LinearNumericCondition &lnc);


/* NumericTaskProxy */
class NumericTaskProxy {
public:
    explicit NumericTaskProxy(const std::shared_ptr<AbstractTask> task);

    const std::vector<ap_float> &get_action_eff_list(int op_id) const {
        return actions[op_id].eff_list;
    }

    const std::vector<std::pair<int, ap_float>> &get_action_assign_list(int op_id) const {
        return actions[op_id].asgn_effs;
    }

    bool is_derived_numeric_variable(const VariableProxy &var_proxy) const;

    int get_regular_var_id(int num_var_id) const;

    int get_global_var_id(int regular_num_var_id) const;

    int get_number_propositional_variables() const;

    int get_number_regular_numeric_variables() const;

    const numeric_condition::RegularNumericCondition &get_regular_numeric_condition(const FactProxy &condition) const;

    const std::vector<numeric_condition::RegularNumericCondition> &get_numeric_goals() const;

    const std::vector<FactProxy> &get_propositional_goals() const;

    int get_approximate_domain_size(NumericVariableProxy num_var);

    bool is_derived_variable(VariableProxy var) const;

    const TaskProxy &get_task_proxy() const {
        // TODO try to get rid of this
        return task_proxy;
    }

    int get_num_variables() const {
        // TODO adapt this
        return task->get_num_variables();
    }

    int get_num_numeric_variables() const {
        // TODO adapt this
        return task->get_num_numeric_variables();
    }

    VariablesProxy get_variables() const {
        // TODO adapt this
        return task_proxy.get_variables();
    }

    NumericVariablesProxy get_numeric_variables() const {
        // TODO adapt this
        return task_proxy.get_numeric_variables();
    }

    OperatorsProxy get_operators() const {
        // TODO adapt this
        return task_proxy.get_operators();
    }

    State get_initial_state() const {
        // TODO adapt this
        return task_proxy.get_initial_state();
    }

    const numeric_pdbs::CausalGraph &get_numeric_causal_graph() const;

    static void verify_is_restricted_numeric_task(const TaskProxy &task_proxy);

private:
    const std::shared_ptr<AbstractTask> task;
    const TaskProxy task_proxy;

    void build_numeric_variables();
    void build_artificial_variables();

    void find_derived_numeric_variables();

    void build_action(const OperatorProxy &op, size_t op_id);
    void build_actions();

    std::shared_ptr<numeric_condition::RegularNumericCondition> build_condition(FactProxy pre);
    void build_preconditions();

    void build_goals();

    std::shared_ptr<arithmetic_expression::ArithmeticExpression> parse_arithmetic_expression(NumericVariableProxy num_var) const;


    std::vector<std::vector<std::shared_ptr<numeric_condition::RegularNumericCondition>>> regular_numeric_conditions;
    std::vector<numeric_condition::RegularNumericCondition> regular_numeric_goals;

    std::vector<FactProxy> propositional_goals;

    std::vector<int> approximate_num_var_domain_sizes;

    // numeric variables
    size_t n_numeric_variables;  // number of regular numeric variables
    std::vector<int> reg_num_var_id_to_glob_var_id; // map regular numeric variable id to global variable id
    std::vector<int> glob_var_id_to_reg_num_var_id; // map global variable id to regular numeric variable id
    std::vector<LinearNumericCondition> artificial_variables;

    std::vector<bool> is_derived_num_var; // true for propositional variables that encodes derived numeric facts

    std::vector<Action> actions;
};
}
#endif
