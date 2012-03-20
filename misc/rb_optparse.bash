#! /bin/bash
# Completion for bash:
#
# (1) install this file,
#
# (2) load the script, and
#      . ~/.profile.d/rb_optparse.bash
#
# (3) define completions in your .bashrc,
#      rb_optparse command_using_optparse_1
#      rb_optparse command_using_optparse_2

_rb_optparse() {
  COMPREPLY=($("${COMP_WORDS[0]}" "--*-completion-bash=${COMP_WORDS[COMP_CWORD]}"))
  return 0
}

rb_optparse () {
    [ $# = 0 ] || complete -o default -F _rb_optparse "$@"
}
