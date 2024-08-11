#ifndef NUMERIC_PDBS_ARITHMETIC_EXPRESSION_H
#define NUMERIC_PDBS_ARITHMETIC_EXPRESSION_H

#include "../globals.h"
#include "../task_proxy.h"

#include <cassert>

namespace numeric_pdb_helper {
class NumericTaskProxy;
}

namespace arithmetic_expression {

class ArithmeticExpression {
    // an arithmetic expression over a single regular numeric variable and constants

public:
    virtual ~ArithmeticExpression() = default;

    virtual int get_var_id() const = 0;

    virtual std::string get_name() const = 0;

    virtual ap_float evaluate(ap_float value) const = 0;

    // this expects an entry for every numeric variable, not just regular ones
    virtual ap_float evaluate(const std::vector<ap_float> &num_values) const = 0;

    // this expects an entry for every numeric variable, not just regular ones
    virtual ap_float evaluate_ignore_additive_consts(const std::vector<ap_float> &num_values) const = 0;

    virtual ap_float evaluate(const State &state, const numeric_pdb_helper::NumericTaskProxy &task) const = 0;

    virtual std::shared_ptr<ArithmeticExpression> simplify() = 0;

    virtual ap_float get_multiplier() const = 0;

    virtual ap_float get_summand() const = 0;

    virtual bool is_constant() const = 0;
};

class ArithmeticExpressionVar : public ArithmeticExpression {
    // this class encodes the basic expressions with just a variable
    int var_id; // global id of a regular numeric variable

public:
    explicit ArithmeticExpressionVar(int var_id)
            : var_id(var_id) {
        assert(var_id >= 0);
        assert(static_cast<size_t>(var_id) >= g_numeric_var_types.size() ||
               g_numeric_var_types[var_id] == numType::regular);
    };

    int get_var_id() const override {
        return var_id;
    }

    std::string get_name() const override {
        return "var" + std::to_string(var_id);
    }

    ap_float evaluate(ap_float value) const override {
        return value;
    }

    ap_float evaluate(const std::vector<ap_float> &num_values) const override {
        assert(static_cast<size_t>(var_id) < num_values.size());
        return num_values[var_id];
    }

    ap_float evaluate_ignore_additive_consts(const std::vector<ap_float> &num_values) const override {
        return num_values[var_id];
    }

    ap_float evaluate(const State &state, const numeric_pdb_helper::NumericTaskProxy &task) const override;

    std::shared_ptr<ArithmeticExpression> simplify() override {
        return std::make_shared<ArithmeticExpressionVar>(*this);
    }

    ap_float get_multiplier() const override {
        return 1;
    }

    ap_float get_summand() const override {
        return 0;
    }

    bool is_constant() const override {
        return false;
    }
};

class ArithmeticExpressionConst : public ArithmeticExpression {
    // this class encodes the basic expressions with just a constant
    ap_float const_;

public:
    explicit ArithmeticExpressionConst(ap_float const_)
            : const_(const_) {
    }

    int get_var_id() const override {
        return -1;
    }

    std::string get_name() const override {
        return std::to_string(const_);
    }

    ap_float evaluate(ap_float /*value*/) const override {
        return const_;
    }

    ap_float evaluate(const std::vector<ap_float> &/*num_values*/) const override {
        return const_;
    }

    ap_float evaluate_ignore_additive_consts(const std::vector<ap_float> &/*num_values*/) const override {
        return const_;
    }

    ap_float evaluate(const State &/*state*/, const numeric_pdb_helper::NumericTaskProxy &/*task*/) const override {
        return const_;
    }

    std::shared_ptr<ArithmeticExpression> simplify() override {
        return std::make_shared<ArithmeticExpressionConst>(*this);
    }

    ap_float get_multiplier() const override {
        return 1;
    }

    ap_float get_summand() const override {
        return const_;
    }

    bool is_constant() const override {
        return true;
    }
};

class ArithmeticExpressionOp : public ArithmeticExpression {
    std::shared_ptr<ArithmeticExpression> lhs;
    cal_operator c_op;
    std::shared_ptr<ArithmeticExpression> rhs;

public:
    ArithmeticExpressionOp(std::shared_ptr<ArithmeticExpression> lhs,
                           cal_operator c_op,
                           std::shared_ptr<ArithmeticExpression> rhs)
            : lhs(std::move(lhs)), c_op(c_op), rhs(std::move(rhs)) {
    }

    int get_var_id() const override {
        int var_id = lhs->get_var_id();
        if (var_id != -1){
            assert(rhs->get_var_id() == -1); // there can only be one variable in the expression
            return var_id;
        } else {
            return rhs->get_var_id();
        }
    }

    std::string get_name() const override;

    ap_float evaluate(ap_float value) const override;

    ap_float evaluate(const std::vector<ap_float> &num_values) const override;

    ap_float evaluate_ignore_additive_consts(const std::vector<ap_float> &num_values) const override;

    ap_float evaluate(const State &state, const numeric_pdb_helper::NumericTaskProxy &task) const override;

    std::shared_ptr<ArithmeticExpression> simplify() override {
        if (lhs->get_var_id() == -1 && rhs->get_var_id() == -1){
            return std::make_shared<ArithmeticExpressionConst>(evaluate(0));
        }
        lhs = lhs->simplify();
        rhs = rhs->simplify();
        return std::make_shared<ArithmeticExpressionOp>(*this); // TODO simplify further
    }

    ap_float get_multiplier() const override;

    ap_float get_summand() const override;

    bool is_constant() const override {
        return lhs->is_constant() && rhs->is_constant();
    }
};
}

#endif
