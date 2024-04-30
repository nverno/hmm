#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <dlfcn.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <emacs-module.h>
#include <tree_sitter/api.h>

namespace fs = std::filesystem;

typedef struct emacs_env_30 emacs_env;
typedef void *dynlib_handle_ptr;

// #define ATTRIBUTE_MAY_ALIAS __attribute__((__may_alias__))
// typedef void(ATTRIBUTE_MAY_ALIAS *dynlib_function_ptr)(void);

int plugin_is_GPL_compatible;

static void tsmeta_symbol_to_c_name(std::string &s) {
  for (char &c : s) {
    if (c == '-')
      c = '_';
  }
}

// load parser dynlib
static TSLanguage *tsmeta_load_language(emacs_env *env, const char *path) {
  const char *error;
  dynlib_handle_ptr handle = dlopen(path, RTLD_LAZY | RTLD_GLOBAL);
  error = dlerror();
  if (error != NULL) {
    static const char message[] = "dlopen failed";
    emacs_value data = env->make_string(env, message, strlen(message));
    env->funcall(env, env->intern(env, "message"), 1, &data);
    // std::cerr << "dlopen failed" << std::endl;
    return NULL;
  }

  TSLanguage *(*langfn)(void);
  fs::path p(path);
  auto lib_name = p.filename().stem().string();
  tsmeta_symbol_to_c_name(lib_name);
  langfn = (TSLanguage * (*)(void)) dlsym(handle, lib_name.substr(3).c_str());
  if (dlerror() != NULL) {
    static const char message[] = "dlsym failed";
    emacs_value data = env->make_string(env, message, strlen(message));
    env->funcall(env, env->intern(env, "message"), 1, &data);

    return NULL;
  }

  TSLanguage *lang = (*langfn)();
  return lang;
}

static int tsmeta_close(dynlib_handle_ptr h) { return dlclose(h) == 0; }

static int string_bytes(emacs_env *env, emacs_value string) {
  ptrdiff_t size = 0;
  env->copy_string_contents(env, string, NULL, &size);
  return size;
}

emacs_value tsmeta_language_grammar(emacs_env *env, ptrdiff_t nargs,
                                    emacs_value args[], void *data) noexcept {
  ptrdiff_t sz = string_bytes(env, args[0]);
  unsigned char p[sz];
  env->copy_string_contents(env, args[0], (char *)p, &sz);

  TSLanguage *lang = tsmeta_load_language(env, (char *)p);

  if (lang == NULL) {
    return env->make_integer(env, -1);
  }

  // Language symbols
  // - ts_language_symbol_count
  // - ts_language_symbol_type
  // - ts_language_symbol_name
  uint32_t nsymbols = ts_language_symbol_count(lang);
  std::vector<std::string> syms;
  for (int i = 0; i < nsymbols; ++i) {
    TSSymbolType t = ts_language_symbol_type(lang, (TSSymbol)i);
    if (t == TSSymbolTypeAuxiliary) {
      // not used by the API
      continue;
    }

    syms.push_back(ts_language_symbol_name(lang, (TSSymbol)i));
  }

  // Language fields
  // - ts_language_field_count
  // - ts_language_field_name_for_id
  uint32_t nfields = ts_language_field_count(lang);
  for (int i = 0; i < nfields; ++i) {
    auto field = ts_language_field_name_for_id(lang, (TSFieldId)i);
  }

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
