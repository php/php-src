require "mkmf"

$readline_headers = ["stdio.h"]

def have_readline_header(header)
  if have_header(header, &$readline_extra_check)
    $readline_headers.push(header)
    return true
  else
    return false
  end
end

def have_readline_var(var)
  return have_var(var, $readline_headers)
end

def have_readline_func(func)
  return have_func(func, $readline_headers)
end

def have_readline_macro(macro)
  return have_macro(macro, $readline_headers)
end

dir_config('curses')
dir_config('ncurses')
dir_config('termcap')
dir_config("readline")
enable_libedit = enable_config("libedit")

have_library("user32", nil) if /cygwin/ === RUBY_PLATFORM
have_library("ncurses", "tgetnum") ||
  have_library("termcap", "tgetnum") ||
  have_library("curses", "tgetnum")

case enable_libedit
when true
  # --enable-libedit
  unless (have_readline_header("editline/readline.h") ||
          have_readline_header("readline/readline.h")) &&
          have_library("edit", "readline")
    exit
  end
when false
  # --disable-libedit
  unless ((have_readline_header("readline/readline.h") &&
           have_readline_header("readline/history.h")) &&
           have_library("readline", "readline"))
    exit
  end
else
  # does not specify
  unless ((have_readline_header("readline/readline.h") &&
           have_readline_header("readline/history.h")) &&
           (have_library("readline", "readline") ||
            have_library("edit", "readline"))) ||
            (have_readline_header("editline/readline.h") &&
             have_library("edit", "readline"))
    exit
  end
end

have_readline_func("rl_getc")
have_readline_func("rl_getc_function")
have_readline_func("rl_filename_completion_function")
have_readline_func("rl_username_completion_function")
have_readline_func("rl_completion_matches")
have_readline_func("rl_refresh_line")
have_readline_var("rl_deprep_term_function")
have_readline_var("rl_completion_append_character")
have_readline_var("rl_basic_word_break_characters")
have_readline_var("rl_completer_word_break_characters")
have_readline_var("rl_basic_quote_characters")
have_readline_var("rl_completer_quote_characters")
have_readline_var("rl_filename_quote_characters")
have_readline_var("rl_attempted_completion_over")
have_readline_var("rl_library_version")
have_readline_var("rl_editing_mode")
have_readline_var("rl_line_buffer")
have_readline_var("rl_point")
# workaround for native windows.
/mswin|bccwin|mingw/ !~ RUBY_PLATFORM && have_readline_var("rl_event_hook")
/mswin|bccwin|mingw/ !~ RUBY_PLATFORM && have_readline_var("rl_catch_sigwinch")
/mswin|bccwin|mingw/ !~ RUBY_PLATFORM && have_readline_var("rl_catch_signals")
have_readline_func("rl_cleanup_after_signal")
have_readline_func("rl_free_line_state")
have_readline_func("rl_clear_signals")
have_readline_func("rl_set_screen_size")
have_readline_func("rl_get_screen_size")
have_readline_func("rl_vi_editing_mode")
have_readline_func("rl_emacs_editing_mode")
have_readline_func("replace_history_entry")
have_readline_func("remove_history")
have_readline_func("clear_history")
have_readline_macro("RL_PROMPT_START_IGNORE")
have_readline_macro("RL_PROMPT_END_IGNORE")
create_makefile("readline")
