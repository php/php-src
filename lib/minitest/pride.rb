######################################################################
# This file is imported from the minitest project.
# DO NOT make modifications in this repo. They _will_ be reverted!
# File a patch instead and assign it to Ryan Davis.
######################################################################

require "minitest/unit"

##
# Show your testing pride!

class PrideIO
  ESC = "\e["
  NND = "#{ESC}0m"

  attr_reader :io

  def initialize io
    @io = io
    # stolen from /System/Library/Perl/5.10.0/Term/ANSIColor.pm
    # also reference http://en.wikipedia.org/wiki/ANSI_escape_code
    @colors ||= (31..36).to_a
    @size   = @colors.size
    @index  = 0
    # io.sync = true
  end

  def print o
    case o
    when "." then
      io.print pride o
    when "E", "F" then
      io.print "#{ESC}41m#{ESC}37m#{o}#{NND}"
    else
      io.print o
    end
  end

  def puts(*o)
    o.map! { |s|
      s.sub(/Finished tests/) {
        @index = 0
        'Fabulous tests'.split(//).map { |c|
          pride(c)
        }.join
      }
    }

    super
  end

  def pride string
    string = "*" if string == "."
    c = @colors[@index % @size]
    @index += 1
    "#{ESC}#{c}m#{string}#{NND}"
  end

  def method_missing msg, *args
    io.send(msg, *args)
  end
end

class PrideLOL < PrideIO # inspired by lolcat, but massively cleaned up
  PI_3 = Math::PI / 3

  def initialize io
    # walk red, green, and blue around a circle separated by equal thirds.
    #
    # To visualize, type this into wolfram-alpha:
    #
    #   plot (3*sin(x)+3), (3*sin(x+2*pi/3)+3), (3*sin(x+4*pi/3)+3)

    # 6 has wide pretty gradients. 3 == lolcat, about half the width
    @colors = (0...(6 * 7)).map { |n|
      n *= 1.0 / 6
      r  = (3 * Math.sin(n           ) + 3).to_i
      g  = (3 * Math.sin(n + 2 * PI_3) + 3).to_i
      b  = (3 * Math.sin(n + 4 * PI_3) + 3).to_i

      # Then we take rgb and encode them in a single number using base 6.
      # For some mysterious reason, we add 16... to clear the bottom 4 bits?
      # Yes... they're ugly.

      36 * r + 6 * g + b + 16
    }

    super
  end

  def pride string
    c = @colors[@index % @size]
    @index += 1
    "#{ESC}38;5;#{c}m#{string}#{NND}"
  end
end

klass = ENV['TERM'] =~ /^xterm|-256color$/ ? PrideLOL : PrideIO
MiniTest::Unit.output = klass.new(MiniTest::Unit.output)
