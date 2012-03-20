#! /usr/bin/ruby
require 'rbconfig'
require 'optparse'

# http://www.ctan.org/get/macros/texinfo/texinfo/gnulib/lib/config.charset
# Tue, 25 Dec 2007 00:00:00 GMT

HEADER = <<SRC
require 'iconv.so'

class Iconv
  case RUBY_PLATFORM
SRC

def charset_alias(config_charset, mapfile = nil)
  found = nil
  src = [HEADER]
  open(config_charset) do |input|
    input.find {|line| /^case "\$os" in/ =~ line} or return
    input.each do |line|
      case line
      when /^\s*([-\w\*]+(?:\s*\|\s*[-\w\*]+)*)(?=\))/
        (s = "  when ") << $&.split('|').collect {|targ|
          targ.strip!
          tail = targ.chomp!("*") ? '' : '\z'
          head = targ.slice!(/\A\*/) ? '' : '\A'
          targ.gsub!(/\*/, '.*')
          "/#{head}#{targ}#{tail}/"
        }.join(", ")
        src << s
        found = {}
      when /^\s*echo "(?:\$\w+\.)?([-\w*]+)\s+([-\w]+)"/
        sys, can = $1, $2
        can.downcase!
        unless found[can] or (/\Aen_(?!US\z)/ =~ sys && /\ACP437\z/i =~ can)
          found[can] = true
          src << "    charset_map['#{can}'] = '#{sys}'.freeze"
        end
      when /^\s*;;/
        found = nil
      end
    end
  end
  src << "  end" << "end"
  if mapfile
    open(mapfile, "wb") {|f| f.puts(*src)}
  else
    puts(*src)
  end
end

(1..2) === ARGV.size or abort "usage: #{$0} config_charset [mapfile]"
charset_alias(*ARGV)
