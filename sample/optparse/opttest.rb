#!/usr/bin/ruby -I.

require 'optparse'
require 'optparse/time'
require 'pp'

# keywords
CODES = %w[iso-2022-jp shift_jis euc-jp utf8 binary]
CODE_ALIASES = {"jis" => "iso-2022-jp", "sjis" => "shift_jis"}
POSSIBLE_CODES = "(#{(CODES+CODE_ALIASES.keys).join(',')})"

ARGV.options do
  |opts|
  opts.banner << " argv..."

  # separator
  opts.on_tail
  opts.on_tail("common options:")

  # no argument, shows at tail
  opts.on_tail("--help", "show this message") {puts opts; exit}

  # mandatory argument
  opts.on("-r", "--require=LIBRARY", String,
	  "require the LIBRARY, before",
	  "executing your script") {|lib|@library=lib}

  # optional argument
  opts.on("-i", "--inplace=[EXTENSION]",
	  "edit ARGV files in place", # multiline description
	  "(make backup if EXTENSION supplied)") {|inplace| @inplace = inplace || ''}

  opts.on("-N=[NUM]", Integer) {|num|@number=num}

  # additional class
  opts.on("-t", "--[no-]time[=TIME]", Time, "it's the time") {|time|@time=time}

  # limit argument syntax
  opts.on("-[0-7]", "-F", "--irs=[OCTAL]", OptionParser::OctalInteger,
	  "specify record separator", "(\\0, if no argument)") {|irs|@irs=irs}

  # boolean switch(default true)
  @exec = true
  opts.on("-n", "--no-exec[=FLAG]", TrueClass, "not really execute") {|exec|@exec=exec}

  # array
  opts.on("-a", "--list[=LIST,LIST]", Array, "list") {|list|@list=list}

  # fixed size array
  opts.on("--pair[=car,cdr]", Array, "pair") {|x,y|@x=x; @y=y}

  # keyword completion
  opts.on("--code=CODE", CODES, CODE_ALIASES, "select coding system",
	  "("+CODES.join(",")+",", " "+CODE_ALIASES.keys.join(",")+")") {|c|@code=c}

  # optional argument with keyword completion
  opts.on("--type[=TYPE]", [:text, :binary], "select type(text, binary)") {|t|@type=t}

  # boolean switch with optional argument(default false)
  opts.on("-v", "--[no-]verbose=[FLAG]", "run verbosely") {|v|@verbose=v}

  # easy way, set local variable
  opts.on("-q", "--quit", "quit when ARGV is empty") {|q|@quit=q}

  # adding on the fly
  opts.on("--add=SWITCH=[ARG]", "add option on the fly", /\A(\w+)(?:=.+)?\Z/) do
    |opt, var|
    opts.on("--#{opt}", "added in runtime", &eval("proc {|x|@#{var}=x}"))
  end

  opts.on_head("specific options:")

  # no argument
  opts.on_tail("--version", "show version") do
    puts OptionParser::Version.join('.')
    exit
  end
  opts.parse!
end

pp self
begin print ARGV.options; exit end if @quit
ARGV.options = nil		# no more parse
puts "ARGV = #{ARGV.join(' ')}" if !ARGV.empty?
#opts.variable.each {|sym| puts "#{sym} = #{opts.send(sym).inspect}"}
