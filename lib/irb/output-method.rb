#
#   output-method.rb - output methods used by irb
#   	$Release Version: 0.9.6$
#   	$Revision$
#   	by Keiju ISHITSUKA(keiju@ruby-lang.org)
#
# --
#
#
#

require "e2mmap"

module IRB
  # OutputMethod
  #   StdioOutputMethod

  class OutputMethod
    @RCS_ID='-$Id$-'

    def print(*opts)
      IRB.fail NotImplementError, "print"
    end

    def printn(*opts)
      print opts.join(" "), "\n"
    end

    # extend printf
    def printf(format, *opts)
      if /(%*)%I/ =~ format
	format, opts = parse_printf_format(format, opts)
      end
      print sprintf(format, *opts)
    end

    # %
    # <flag>  [#0- +]
    # <minimum field width> (\*|\*[1-9][0-9]*\$|[1-9][0-9]*)
    # <precision>.(\*|\*[1-9][0-9]*\$|[1-9][0-9]*|)?
    # #<length modifier>(hh|h|l|ll|L|q|j|z|t)
    # <conversion specifier>[diouxXeEfgGcsb%]
    def parse_printf_format(format, opts)
      return format, opts if $1.size % 2 == 1
    end

    def puts(*objs)
      for obj in objs
	print(*obj)
	print "\n"
      end
    end

    def pp(*objs)
      puts(*objs.collect{|obj| obj.inspect})
    end

    def ppx(prefix, *objs)
      puts(*objs.collect{|obj| prefix+obj.inspect})
    end

  end

  class StdioOutputMethod<OutputMethod
    def print(*opts)
      STDOUT.print(*opts)
    end
  end
end
