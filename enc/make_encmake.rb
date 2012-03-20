#! ./miniruby

dir = File.expand_path("../..", __FILE__)
$:.unshift(dir)
$:.unshift(".")
if $".grep(/mkmf/).empty?
  $" << "mkmf.rb"
  load File.expand_path("lib/mkmf.rb", dir)
end
require 'erb'

CONFIG["MAKEDIRS"] ||= '$(MINIRUBY) -run -e mkdir -- -p'

BUILTIN_ENCS = []
BUILTIN_TRANSES = []
ENC_PATTERNS = []
NOENC_PATTERNS = []

until ARGV.empty?
  case ARGV[0]
  when /\A--builtin-encs=/
    BUILTIN_ENCS.concat $'.split.map {|e| File.basename(e, '.*') << '.c'}
    ARGV.shift
  when /\A--builtin-transes=/
    BUILTIN_TRANSES.concat $'.split.map {|e| File.basename(e, '.*') }
    ARGV.shift
  when /\A--encs=/
    ENC_PATTERNS.concat $'.split
    ARGV.shift
  when /\A--no-encs=/
    NOENC_PATTERNS.concat $'.split
    ARGV.shift
  else
    break
  end
end

if File.exist?(depend = File.join($srcdir, "depend"))
  erb = ERB.new(File.read(depend), nil, '%')
  erb.filename = depend
  tmp = erb.result(binding)
  dep = "\n#### depend ####\n\n" << depend_rules(tmp).join
else
  dep = ""
end
mkin = File.read(File.join($srcdir, "Makefile.in"))
mkin.gsub!(/@(#{CONFIG.keys.join('|')})@/) {CONFIG[$1]}
open(ARGV[0], 'wb') {|f|
  f.puts mkin, dep
}
