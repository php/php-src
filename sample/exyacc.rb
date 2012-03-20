#! /usr/local/bin/ruby -Kn
# usage: exyacc.rb [yaccfiles]
# this is coverted from exyacc.pl in the camel book

ARGF.each(nil) do |source|
  sbeg = source.index("\n%%") + 1
  send = source.rindex("\n%%") + 1
  grammer = source[sbeg, send-sbeg]
  grammer.sub!(/.*\n/, "")
  grammer.gsub!(/'\{'/, "'\001'")
  grammer.gsub!(/'\}'/, "'\002'")
  grammer.gsub!(%r{\*/}, "\003\003")
  grammer.gsub!(%r{/\*[^\003]*\003\003}, '')
  while grammer.gsub!(/\{[^{}]*\}/, ''); end
  grammer.gsub!(/'\001'/, "'{'")
  grammer.gsub!(/'\002'/, "'}'")
  while grammer.gsub!(/^[ \t]*\n(\s)/, '\1'); end
  grammer.gsub!(/([:|])[ \t\n]+(\w)/, '\1 \2')
  print grammer
end
