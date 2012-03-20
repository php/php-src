#! /usr/bin/ruby
# :stopdoc:
require 'rbconfig'
require 'optparse'

# http://www.ctan.org/get/macros/texinfo/texinfo/gnulib/lib/config.charset
# Tue, 25 Dec 2007 00:00:00 GMT

OS = RbConfig::CONFIG["target_os"]
SHELL = RbConfig::CONFIG['SHELL']

class Hash::Ordered < Hash
  def [](key)
    val = super and val.last
  end
  def []=(key, val)
    ary = fetch(key) {return super(key, [self.size, key, val])} and
      ary << val
  end
  def sort
    values.sort.collect {|i, *rest| rest}
  end
  def each(&block)
    sort.each(&block)
  end
end

def charset_alias(config_charset, mapfile, target = OS)
  map = Hash::Ordered.new
  comments = []
  open(config_charset) do |input|
    input.find {|line| /^case "\$os" in/ =~ line} or break
    input.find {|line|
      /^\s*([-\w\*]+(?:\s*\|\s*[-\w\*]+)*)(?=\))/ =~ line and
      $&.split('|').any? {|pattern| File.fnmatch?(pattern.strip, target)}
    } or break
    input.find do |line|
      case line
      when /^\s*echo "(?:\$\w+\.)?([-\w*]+)\s+([-\w]+)"/
        sys, can = $1, $2
        can.downcase!
        map[can] = sys
        false
      when /^\s*;;/
        true
      else
        false
      end
    end
  end
  case target
  when /linux|-gnu/
    # map.delete('ascii')
  when /cygwin|os2-emx/
    # get rid of tilde/yen problem.
    map['shift_jis'] = 'cp932'
  end
  st = Hash.new(0)
  map = map.sort.collect do |can, *sys|
    if sys.grep(/^en_us(?=.|$)/i) {break true} == true
      noen = %r"^(?!en_us)\w+_\w+#{Regexp.new($')}$"i #"
      sys.reject! {|s| noen =~ s}
    end
    sys = sys.first
    st[sys] += 1
    [can, sys]
  end
  st.delete_if {|sys, i| i == 1}.empty?
  st.keys.each {|sys| st[sys] = nil}
  st.default = nil
  writer = proc do |f|
    f.puts("require 'iconv.so'")
    f.puts
    f.puts(comments)
    f.puts("class Iconv")
    i = 0
    map.each do |can, sys|
      if s = st[sys]
        sys = s
      elsif st.key?(sys)
        sys = (st[sys] = "sys#{i+=1}") + " = '#{sys}'.freeze"
      else
        sys = "'#{sys}'.freeze"
      end
      f.puts("  charset_map['#{can}'] = #{sys}")
    end
    f.puts("end")
  end
  if mapfile
    open(mapfile, "w", &writer)
  else
    writer[STDOUT]
  end
end

target = OS
opt = nil
ARGV.options do |opt2|
  opt = opt2
  opt.banner << " config.status map.rb"
  opt.on("--target OS") {|t| target = t}
  opt.parse! and (1..2) === ARGV.size
end or abort opt.to_s
charset_alias(ARGV[0], ARGV[1], target)
