#include "pdb_heuristic.h"

#include "numeric_helper.h"
#include "pattern_generator.h"

#include "../option_parser.h"
#include "../plugin.h"

#include <limits>
#include <memory>

using namespace std;
using numeric_pdb_helper::NumericTaskProxy;

namespace numeric_pdbs {
PatternDatabase get_pdb_from_options(const shared_ptr<AbstractTask> &task,
                                     const Options &opts) {
    auto pattern_generator =
        opts.get<shared_ptr<PatternGenerator>>("pattern");
    shared_ptr<NumericTaskProxy> task_proxy = make_shared<NumericTaskProxy>(task);
    Pattern pattern = pattern_generator->generate(task, task_proxy);
    return {task_proxy, pattern, pattern_generator->get_max_number_pdb_states(), true};
}

NumericPDBHeuristic::NumericPDBHeuristic(const Options &opts)
    : Heuristic(opts),
      pdb(get_pdb_from_options(task, opts)),
      number_lookup_misses(0) {
}

ap_float NumericPDBHeuristic::compute_heuristic(const GlobalState &global_state) {
    State state = convert_global_state(global_state);
    return compute_heuristic(state);
}

ap_float NumericPDBHeuristic::compute_heuristic(const State &state) const {
    auto [found_state, h] = pdb.get_value(state);
    if (!found_state){
        number_lookup_misses++;
    }
    if (h == numeric_limits<ap_float>::max()) {
        return DEAD_END;
    }
    return h;
}

void NumericPDBHeuristic::print_statistics() const {
    cout << "Number of failed heuristic lookups: " << number_lookup_misses << endl;
}

static Heuristic *_parse(OptionParser &parser) {
    parser.document_synopsis("Numeric pattern database heuristic", "TODO");
    parser.document_language_support("action costs", "supported");
    parser.document_language_support("conditional effects", "not supported");
    parser.document_language_support("axioms", "not supported");
    parser.document_property("admissible", "yes");
    parser.document_property("consistent", "yes");
    parser.document_property("safe", "TODO");
    parser.document_property("preferred operators", "no");

    parser.add_option<shared_ptr<PatternGenerator>>(
        "pattern",
        "pattern generation method",
        "greedy_numeric()");

    Heuristic::add_options_to_parser(parser);

    Options opts = parser.parse();
    if (parser.dry_run())
        return nullptr;

    return new NumericPDBHeuristic(opts);
}

static Plugin<Heuristic> _plugin("numeric_pdb", _parse);
}
