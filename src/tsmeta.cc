#include <cstddef>
#include <cstdlib>
#include <emacs-module.h>
#include <fstream>
#include <string>
#include <tree_sitter/api.h>

typedef struct emacs_env_30 emacs_env;

int plugin_is_GPL_compatible;

// emacs_value Qnil;

// TODO: load parser dynlib
// static TSLanguage *tsq_load_language(
//   Lisp_Object language_symbol,
//   Lisp_Object *signal_symbol,
//   Lisp_Object *signal_data
// ) {}

emacs_value tsmeta_language_grammar(emacs_env *env, ptrdiff_t nargs,
                                     emacs_value args[], void *data) noexcept {
  // TODO: inspect language
  // - ts_language_symbol_count
  // - ts_language_symbol_type
  // - ts_language_symbol_name
  // - ts_language_field_count
  // - ts_language_field_name_for_id
  return env->make_integer(env, 0);
}

int emacs_module_init(struct emacs_runtime *runtime) {
  if (runtime->size < sizeof(*runtime))
    return 1;

  emacs_env *env = runtime->get_environment(runtime);

  // Register symbols / functions
  // Qnil = env->intern(env, "nil");
  emacs_value Qfset = env->intern(env, "fset");

  // emacs_value tsmeta_language_grammar =
  //   env->make_global_ref(env, env->intern(env, "tsmeta-language-grammar"));

  {
    emacs_value func =
      env->make_function(env, 1, 1, tsmeta_language_grammar,
        "Get symbols and fields defined by language.", NULL);
    emacs_value sym = env->intern(env, "tsmeta-language-grammar");

    // Set function cell
    emacs_value args[] = {sym, func};
    env->funcall(env, Qfset, 2, args);
  }

  // Provide feature
  emacs_value Qfeat = env->intern(env, "tsmeta");
  emacs_value Qprovide = env->intern(env, "provide");
  emacs_value args[] = {Qfeat};
  env->funcall(env, Qprovide, 1, args);

  return 0;
}

// extern "C" {
// #include "javascript/src/parser.c"
// #include "javascript/src/scanner.c"
// }

// #define LANGUAGE tree_sitter_javascript
// #define SOURCE_PATH "javascript/examples/jquery.js"

// int main() {
//   TSParser *parser = ts_parser_new();
//   if (!ts_parser_set_language(parser, LANGUAGE())) {
//     fprintf(stderr, "Invalid language\n");
//     exit(1);
//   }

//   const char *source_path = GRAMMARS_DIR SOURCE_PATH;

//   printf("Parsing %s\n", source_path);

//   std::ifstream source_file(source_path);
//   if (!source_file.good()) {
//     fprintf(stderr, "Invalid source path %s\n", source_path);
//     exit(1);
//   }

//   std::string source_code((std::istreambuf_iterator<char>(source_file)),
//                           std::istreambuf_iterator<char>());

//   TSTree *tree = ts_parser_parse_string(parser, NULL, source_code.c_str(),
//                                         source_code.size());
// }
