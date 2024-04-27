use std::path::PathBuf;

use emacs::{defun, Env, Result, Value};
use tree_sitter_loader::Loader;

mod types;
mod lang;

emacs::plugin_is_GPL_compatible!();

// Register the initialization hook that Emacs will call when it loads the module.
#[emacs::module(name = "tsq", defun_prefix = "tsq", mod_in_name = false)]
fn init(env: &Env) -> Result<()> {
    // This is run when Emacs loads the module.
    // More concretely, it is run after all the functions it defines are exported,
    // but before `(provide 'feature-name)` is (automatically) called.
    env.message("Done loading!")?;
    Ok(())
}

fn load_parser(env: &Env, path: String) -> Result<()> {
    let mut loader = Loader::new()?;
    let parser_path: PathBuf = path.into();
    loader.find_language_configurations_at_path(&parser_path, false)?;
    Ok(())
}

// Define a function callable by Lisp code.
#[defun]
fn say_hello(env: &Env, name: String) -> Result<Value<'_>> {
    env.message(format!("Hello, {}!", name))
}
