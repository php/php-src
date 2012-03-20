#!/bin/zsh
# Completion for zsh:
# (based on <http://d.hatena.ne.jp/rubikitch/20071002/zshcomplete>)
#
# (1) install this file.
#      mkdir -p ~/.zsh.d
#      cp rb_optparse.zsh ~/.zsh.d/rb_optparse.zsh
#
# (2) load the script, and add a directory to fpath before compinit.
#      echo '. ~/.zsh.d/rb_optparse.zsh' >> ~/.zshrc
#      echo 'fpath=(~/.zsh.d/Completion $fpath)' >> ~/.zshrc
#      echo 'autoload -U compinit; compinit' >> ~/.zshrc
#
# (3) restart zsh.
#
# (4) geneate completion files once.
#      generate-complete-function/ruby/optparse COMMAND1
#      generate-complete-function/ruby/optparse COMMAND2
#

generate-complete-function/ruby/optparse ()
{
    local cmpl="_${1:t}"
    mkdir -p "${ZSH_COMPLETION_DIR-$HOME/.zsh.d/Completion}"
    $1 "--*-completion-zsh=${1:t}" >! "${ZSH_COMPLETION_DIR-$HOME/.zsh.d/Completion}/$cmpl"
    if [[ $(type -w "$cmpl") == "${cmpl}: function" ]]; then
	unfunction "$cmpl"
	autoload -U "$cmpl"
    else
        compinit "$cmpl"
    fi
}

compdef _command generate-complete-function/ruby/optparse

for cmd in "$@"; do
    generate-complete-function/ruby/optparse "$cmd"
done
