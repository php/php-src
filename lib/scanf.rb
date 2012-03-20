# scanf for Ruby
#
#--
# $Release Version: 1.1.2 $
# $Revision$
# $Id$
# $Author$
#++
#
# == Description
#
# scanf is an implementation of the C function scanf(3), modified as necessary
# for ruby compatibility.
#
# the methods provided are String#scanf, IO#scanf, and
# Kernel#scanf. Kernel#scanf is a wrapper around STDIN.scanf.  IO#scanf
# can be used on any IO stream, including file handles and sockets.
# scanf can be called either with or without a block.
#
# Scanf scans an input string or stream according to a <b>format</b>, as
# described below in Conversions, and returns an array of matches between
# the format and the input.  The format is defined in a string, and is
# similar (though not identical) to the formats used in Kernel#printf and
# Kernel#sprintf.
#
# The format may contain <b>conversion specifiers</b>, which tell scanf
# what form (type) each particular matched substring should be converted
# to (e.g., decimal integer, floating point number, literal string,
# etc.)  The matches and conversions take place from left to right, and
# the conversions themselves are returned as an array.
#
# The format string may also contain characters other than those in the
# conversion specifiers.  White space (blanks, tabs, or newlines) in the
# format string matches any amount of white space, including none, in
# the input.  Everything else matches only itself.
#
# Scanning stops, and scanf returns, when any input character fails to
# match the specifications in the format string, or when input is
# exhausted, or when everything in the format string has been
# matched. All matches found up to the stopping point are returned in
# the return array (or yielded to the block, if a block was given).
#
#
# == Basic usage
#
#   require 'scanf'
#
#   # String#scanf and IO#scanf take a single argument, the format string
#   array = a_string.scanf("%d%s")
#   array = an_io.scanf("%d%s")
#
#   # Kernel#scanf reads from STDIN
#   array = scanf("%d%s")
#
# == Block usage
#
# When called with a block, scanf keeps scanning the input, cycling back
# to the beginning of the format string, and yields a new array of
# conversions to the block every time the format string is matched
# (including partial matches, but not including complete failures).  The
# actual return value of scanf when called with a block is an array
# containing the results of all the executions of the block.
#
#   str = "123 abc 456 def 789 ghi"
#   str.scanf("%d%s") { |num,str| [ num * 2, str.upcase ] }
#   # => [[246, "ABC"], [912, "DEF"], [1578, "GHI"]]
#
# == Conversions
#
# The single argument to scanf is a format string, which generally
# includes one or more conversion specifiers.  Conversion specifiers
# begin with the percent character ('%') and include information about
# what scanf should next scan for (string, decimal number, single
# character, etc.).
#
# There may be an optional maximum field width, expressed as a decimal
# integer, between the % and the conversion.  If no width is given, a
# default of `infinity' is used (with the exception of the %c specifier;
# see below).  Otherwise, given a field width of <em>n</em> for a given
# conversion, at most <em>n</em> characters are scanned in processing
# that conversion.  Before conversion begins, most conversions skip
# white space in the input string; this white space is not counted
# against the field width.
#
# The following conversions are available.
#
# [%]
#   Matches a literal `%'. That is, `%%' in the format string matches a
#   single input `%' character. No conversion is done, and the resulting
#   '%' is not included in the return array.
#
# [d]
#   Matches an optionally signed decimal integer.
#
# [u]
#   Same as d.
#
# [i]
#   Matches an optionally signed integer. The integer is read in base
#   16 if it begins with `0x' or `0X', in base 8 if it begins with `0',
#   and in base 10 other- wise. Only characters that correspond to the
#   base are recognized.
#
# [o]
#   Matches an optionally signed octal integer.
#
# [x, X]
#   Matches an optionally signed hexadecimal integer,
#
# [a, e, f, g, A, E, F, G]
#   Matches an optionally signed floating-point number.
#
# [s]
#   Matches a sequence of non-white-space character. The input string stops at
#   white space or at the maximum field width, whichever occurs first.
#
# [c]
#   Matches a single character, or a sequence of <em>n</em> characters if a
#   field width of <em>n</em> is specified. The usual skip of leading white
#   space is suppressed. To skip white space first, use an explicit space in
#   the format.
#
# [[]
#   Matches a nonempty sequence of characters from the specified set
#   of accepted characters.  The usual skip of leading white space is
#   suppressed.  This bracketed sub-expression is interpreted exactly like a
#   character class in a Ruby regular expression.  (In fact, it is placed as-is
#   in a regular expression.)  The matching against the input string ends with
#   the appearance of a character not in (or, with a circumflex, in) the set,
#   or when the field width runs out, whichever comes first.
#
# === Assignment suppression
#
# To require that a particular match occur, but without including the result
# in the return array, place the <b>assignment suppression flag</b>, which is
# the star character ('*'), immediately after the leading '%' of a format
# specifier (just before the field width, if any).
#
# == scanf for Ruby compared with scanf in C
#
# scanf for Ruby is based on the C function scanf(3), but with modifications,
# dictated mainly by the underlying differences between the languages.
#
# === Unimplemented flags and specifiers
#
# * The only flag implemented in scanf for Ruby is '<tt>*</tt>' (ignore
#   upcoming conversion).  Many of the flags available in C versions of
#   scanf(3) have to do with the type of upcoming pointer arguments, and are
#   meaningless in Ruby.
#
# * The <tt>n</tt> specifier (store number of characters consumed so far in
#   next pointer) is not implemented.
#
# * The <tt>p</tt> specifier (match a pointer value) is not implemented.
#
# === Altered specifiers
#
# [o, u, x, X]
#   In scanf for Ruby, all of these specifiers scan for an optionally signed
#   integer, rather than for an unsigned integer like their C counterparts.
#
# === Return values
#
# scanf for Ruby returns an array of successful conversions, whereas
# scanf(3) returns the number of conversions successfully
# completed. (See below for more details on scanf for Ruby's return
# values.)
#
# == Return values
#
# Without a block, scanf returns an array containing all the conversions
# it has found.  If none are found, scanf will return an empty array. An
# unsuccessful match is never ignored, but rather always signals the end
# of the scanning operation.  If the first unsuccessful match takes place
# after one or more successful matches have already taken place, the
# returned array will contain the results of those successful matches.
#
# With a block scanf returns a 'map'-like array of transformations from
# the block -- that is, an array reflecting what the block did with each
# yielded result from the iterative scanf operation.  (See "Block
# usage", above.)
#
# == Current limitations and bugs
#
# When using IO#scanf under Windows, make sure you open your files in
# binary mode:
#
#     File.open("filename", "rb")
#
# so that scanf can keep track of characters correctly.
#
# Support for character classes is reasonably complete (since it
# essentially piggy-backs on Ruby's regular expression handling of
# character classes), but users are advised that character class testing
# has not been exhaustive, and that they should exercise some caution
# in using any of the more complex and/or arcane character class
# idioms.
#
# == License and copyright
#
# Copyright:: (c) 2002-2003 David Alan Black
# License:: Distributed on the same licensing terms as Ruby itself
#
# == Warranty disclaimer
#
# This software is provided "as is" and without any express or implied
# warranties, including, without limitation, the implied warranties of
# merchantability and fitness for a particular purpose.
#
# == Credits and acknowledgements
#
# scanf was developed as the major activity of the Austin Ruby Codefest
# (Austin, Texas, August 2002).
#
# Principal author:: David Alan Black (mailto:dblack@superlink.net)
# Co-author:: Hal Fulton (mailto:hal9000@hypermetrics.com)
# Project contributors:: Nolan Darilek, Jason Johnston
#
# Thanks to Hal Fulton for hosting the Codefest.
#
# Thanks to Matz for suggestions about the class design.
#
# Thanks to Gavin Sinclair for some feedback on the documentation.
#
# The text for parts of this document, especially the Description and
# Conversions sections, above, were adapted from the Linux Programmer's
# Manual manpage for scanf(3), dated 1995-11-01.
#
# == Bugs and bug reports
#
# scanf for Ruby is based on something of an amalgam of C scanf
# implementations and documentation, rather than on a single canonical
# description.  Suggestions for features and behaviors which appear in
# other scanfs, and would be meaningful in Ruby, are welcome, as are
# reports of suspicious behaviors and/or bugs.  (Please see "Credits and
# acknowledgements", above, for email addresses.)

module Scanf
  # :stopdoc:

  # ==Technical notes
  #
  # ===Rationale behind scanf for Ruby
  #
  # The impetus for a scanf implementation in Ruby comes chiefly from the fact
  # that existing pattern matching operations, such as Regexp#match and
  # String#scan, return all results as strings, which have to be converted to
  # integers or floats explicitly in cases where what's ultimately wanted are
  # integer or float values.
  #
  # ===Design of scanf for Ruby
  #
  # scanf for Ruby is essentially a <format string>-to-<regular
  # expression> converter.
  #
  # When scanf is called, a FormatString object is generated from the
  # format string ("%d%s...") argument. The FormatString object breaks the
  # format string down into atoms ("%d", "%5f", "blah", etc.), and from
  # each atom it creates a FormatSpecifier object, which it
  # saves.
  #
  # Each FormatSpecifier has a regular expression fragment and a "handler"
  # associated with it. For example, the regular expression fragment
  # associated with the format "%d" is "([-+]?\d+)", and the handler
  # associated with it is a wrapper around String#to_i. scanf itself calls
  # FormatString#match, passing in the input string. FormatString#match
  # iterates through its FormatSpecifiers; for each one, it matches the
  # corresponding regular expression fragment against the string. If
  # there's a match, it sends the matched string to the handler associated
  # with the FormatSpecifier.
  #
  # Thus, to follow up the "%d" example: if "123" occurs in the input
  # string when a FormatSpecifier consisting of "%d" is reached, the "123"
  # will be matched against "([-+]?\d+)", and the matched string will be
  # rendered into an integer by a call to to_i.
  #
  # The rendered match is then saved to an accumulator array, and the
  # input string is reduced to the post-match substring. Thus the string
  # is "eaten" from the left as the FormatSpecifiers are applied in
  # sequence.  (This is done to a duplicate string; the original string is
  # not altered.)
  #
  # As soon as a regular expression fragment fails to match the string, or
  # when the FormatString object runs out of FormatSpecifiers, scanning
  # stops and results accumulated so far are returned in an array.

  class FormatSpecifier

    attr_reader :re_string, :matched_string, :conversion, :matched

    private

    def skip;  /^\s*%\*/.match(@spec_string); end

    def extract_float(s)
      return nil unless s &&! skip
      if /\A(?<sign>[-+]?)0[xX](?<frac>\.\h+|\h+(?:\.\h*)?)[pP](?<exp>[-+]\d+)/ =~ s
        f1, f2 = frac.split('.')
        f = f1.hex
        if f2
          len = f2.length
          if len > 0
            f += f2.hex / (16.0 ** len)
          end
        end
        (sign == ?- ? -1 : 1) * Math.ldexp(f, exp.to_i)
      elsif /\A([-+]?\d+)\.([eE][-+]\d+)/ =~ s
        ($1 << $2).to_f
      else
        s.to_f
      end
    end
    def extract_decimal(s); s.to_i if s &&! skip; end
    def extract_hex(s); s.hex if s &&! skip; end
    def extract_octal(s); s.oct if s &&! skip; end
    def extract_integer(s); Integer(s) if s &&! skip; end
    def extract_plain(s); s unless skip; end

    def nil_proc(s); nil; end

    public

    def to_s
      @spec_string
    end

    def count_space?
      /(?:\A|\S)%\*?\d*c|%\d*\[/.match(@spec_string)
    end

    def initialize(str)
      @spec_string = str
      h = '[A-Fa-f0-9]'

      @re_string, @handler =
        case @spec_string

          # %[[:...:]]
        when /%\*?(\[\[:[a-z]+:\]\])/
          [ "(#{$1}+)", :extract_plain ]

          # %5[[:...:]]
        when /%\*?(\d+)(\[\[:[a-z]+:\]\])/
          [ "(#{$2}{1,#{$1}})", :extract_plain ]

          # %[...]
        when /%\*?\[([^\]]*)\]/
          yes = $1
          if /^\^/.match(yes) then no = yes[1..-1] else no = '^' + yes end
          [ "([#{yes}]+)(?=[#{no}]|\\z)", :extract_plain ]

          # %5[...]
        when /%\*?(\d+)\[([^\]]*)\]/
          yes = $2
          w = $1
          [ "([#{yes}]{1,#{w}})", :extract_plain ]

          # %i
        when /%\*?i/
          [ "([-+]?(?:(?:0[0-7]+)|(?:0[Xx]#{h}+)|(?:[1-9]\\d*)))", :extract_integer ]

          # %5i
        when /%\*?(\d+)i/
          n = $1.to_i
          s = "("
          if n > 1 then s += "[1-9]\\d{1,#{n-1}}|" end
          if n > 1 then s += "0[0-7]{1,#{n-1}}|" end
          if n > 2 then s += "[-+]0[0-7]{1,#{n-2}}|" end
          if n > 2 then s += "[-+][1-9]\\d{1,#{n-2}}|" end
          if n > 2 then s += "0[Xx]#{h}{1,#{n-2}}|" end
          if n > 3 then s += "[-+]0[Xx]#{h}{1,#{n-3}}|" end
          s += "\\d"
          s += ")"
          [ s, :extract_integer ]

          # %d, %u
        when /%\*?[du]/
          [ '([-+]?\d+)', :extract_decimal ]

          # %5d, %5u
        when /%\*?(\d+)[du]/
          n = $1.to_i
          s = "("
          if n > 1 then s += "[-+]\\d{1,#{n-1}}|" end
          s += "\\d{1,#{$1}})"
          [ s, :extract_decimal ]

          # %x
        when /%\*?[Xx]/
          [ "([-+]?(?:0[Xx])?#{h}+)", :extract_hex ]

          # %5x
        when /%\*?(\d+)[Xx]/
          n = $1.to_i
          s = "("
          if n > 3 then s += "[-+]0[Xx]#{h}{1,#{n-3}}|" end
          if n > 2 then s += "0[Xx]#{h}{1,#{n-2}}|" end
          if n > 1 then s += "[-+]#{h}{1,#{n-1}}|" end
          s += "#{h}{1,#{n}}"
          s += ")"
          [ s, :extract_hex ]

          # %o
        when /%\*?o/
          [ '([-+]?[0-7]+)', :extract_octal ]

          # %5o
        when /%\*?(\d+)o/
          [ "([-+][0-7]{1,#{$1.to_i-1}}|[0-7]{1,#{$1}})", :extract_octal ]

          # %f
        when /%\*?[aefgAEFG]/
          [ '([-+]?(?:0[xX](?:\.\h+|\h+(?:\.\h*)?)[pP][-+]\d+|\d+(?![\d.])|\d*\.\d*(?:[eE][-+]?\d+)?))', :extract_float ]

          # %5f
        when /%\*?(\d+)[aefgAEFG]/
          [ '(?=[-+]?(?:0[xX](?:\.\h+|\h+(?:\.\h*)?)[pP][-+]\d+|\d+(?![\d.])|\d*\.\d*(?:[eE][-+]?\d+)?))' +
            "(\\S{1,#{$1}})", :extract_float ]

          # %5s
        when /%\*?(\d+)s/
          [ "(\\S{1,#{$1}})", :extract_plain ]

          # %s
        when /%\*?s/
          [ '(\S+)', :extract_plain ]

          # %c
        when /\s%\*?c/
          [ "\\s*(.)", :extract_plain ]

          # %c
        when /%\*?c/
          [ "(.)", :extract_plain ]

          # %5c (whitespace issues are handled by the count_*_space? methods)
        when /%\*?(\d+)c/
          [ "(.{1,#{$1}})", :extract_plain ]

          # %%
        when /%%/
          [ '(\s*%)', :nil_proc ]

          # literal characters
        else
          [ "(#{Regexp.escape(@spec_string)})", :nil_proc ]
        end

      @re_string = '\A' + @re_string
    end

    def to_re
      Regexp.new(@re_string,Regexp::MULTILINE)
    end

    def match(str)
      @matched = false
      s = str.dup
      s.sub!(/\A\s+/,'') unless count_space?
      res = to_re.match(s)
      if res
        @conversion = send(@handler, res[1])
        @matched_string = @conversion.to_s
        @matched = true
      end
      res
    end

    def letter
      @spec_string[/%\*?\d*([a-z\[])/, 1]
    end

    def width
      w = @spec_string[/%\*?(\d+)/, 1]
      w && w.to_i
    end

    def mid_match?
      return false unless @matched
      cc_no_width    = letter == '[' &&! width
      c_or_cc_width  = (letter == 'c' || letter == '[') && width
      width_left     = c_or_cc_width && (matched_string.size < width)

      return width_left || cc_no_width
    end

  end

  class FormatString

    attr_reader :string_left, :last_spec_tried,
                :last_match_tried, :matched_count, :space

    SPECIFIERS = 'diuXxofFeEgGscaA'
    REGEX = /
        # possible space, followed by...
          (?:\s*
          # percent sign, followed by...
            %
            # another percent sign, or...
              (?:%|
                 # optional assignment suppression flag
                 \*?
                 # optional maximum field width
                 \d*
                   # named character class, ...
                   (?:\[\[:\w+:\]\]|
                   # traditional character class, or...
                      \[[^\]]*\]|
                   # specifier letter.
                      [#{SPECIFIERS}])))|
            # or miscellaneous characters
              [^%\s]+/ix

    def initialize(str)
      @specs = []
      @i = 1
      s = str.to_s
      return unless /\S/.match(s)
      @space = true if /\s\z/.match(s)
      @specs.replace s.scan(REGEX).map {|spec| FormatSpecifier.new(spec) }
    end

    def to_s
      @specs.join('')
    end

    def prune(n=matched_count)
      n.times { @specs.shift }
    end

    def spec_count
      @specs.size
    end

    def last_spec
      @i == spec_count - 1
    end

    def match(str)
      accum = []
      @string_left = str
      @matched_count = 0

      @specs.each_with_index do |spec,i|
        @i=i
        @last_spec_tried = spec
        @last_match_tried = spec.match(@string_left)
        break unless @last_match_tried
        @matched_count += 1

        accum << spec.conversion

        @string_left = @last_match_tried.post_match
        break if @string_left.empty?
      end
      return accum.compact
    end
  end
  # :startdoc:
end

class IO

  #:stopdoc:
  # The trick here is doing a match where you grab one *line*
  # of input at a time.  The linebreak may or may not occur
  # at the boundary where the string matches a format specifier.
  # And if it does, some rule about whitespace may or may not
  # be in effect...
  #
  # That's why this is much more elaborate than the string
  # version.
  #
  # For each line:
  #
  # Match succeeds (non-emptily)
  # and the last attempted spec/string sub-match succeeded:
  #
  #   could the last spec keep matching?
  #     yes: save interim results and continue (next line)
  #
  # The last attempted spec/string did not match:
  #
  # are we on the next-to-last spec in the string?
  #   yes:
  #     is fmt_string.string_left all spaces?
  #       yes: does current spec care about input space?
  #         yes: fatal failure
  #         no: save interim results and continue
  #   no: continue  [this state could be analyzed further]
  #
  #:startdoc:

  # Scans the current string until the match is exhausted,
  # yielding each match as it is encountered in the string.
  # A block is not necessary though, as the results will simply
  # be aggregated into the final array.
  #
  #   "123 456".block_scanf("%d")
  #   # => [123, 456]
  #
  # If a block is given, the value from that is returned from
  # the yield is added to an output array.
  #
  #   "123 456".block_scanf("%d) do |digit,| # the ',' unpacks the Array
  #     digit + 100
  #   end
  #   # => [223, 556]
  #
  # See Scanf for details on creating a format string.
  #
  # You will need to require 'scanf' to use use IO#scanf.
  def scanf(str,&b) #:yield: current_match
    return block_scanf(str,&b) if b
    return [] unless str.size > 0

    start_position = pos rescue 0
    matched_so_far = 0
    source_buffer = ""
    result_buffer = []
    final_result = []

    fstr = Scanf::FormatString.new(str)

    loop do
      if eof || (tty? &&! fstr.match(source_buffer))
        final_result.concat(result_buffer)
        break
      end

      source_buffer << gets

      current_match = fstr.match(source_buffer)

      spec = fstr.last_spec_tried

      if spec.matched
        if spec.mid_match?
          result_buffer.replace(current_match)
          next
        end

      elsif (fstr.matched_count == fstr.spec_count - 1)
        if /\A\s*\z/.match(fstr.string_left)
          break if spec.count_space?
          result_buffer.replace(current_match)
          next
        end
      end

      final_result.concat(current_match)

      matched_so_far += source_buffer.size
      source_buffer.replace(fstr.string_left)
      matched_so_far -= source_buffer.size
      break if fstr.last_spec
      fstr.prune
    end
    seek(start_position + matched_so_far, IO::SEEK_SET) rescue Errno::ESPIPE
    soak_up_spaces if fstr.last_spec && fstr.space

    return final_result
  end

  private

  def soak_up_spaces
    c = getc
    ungetc(c) if c
    until eof ||! c || /\S/.match(c.chr)
      c = getc
    end
    ungetc(c) if (c && /\S/.match(c.chr))
  end

  def block_scanf(str)
    final = []
# Sub-ideal, since another FS gets created in scanf.
# But used here to determine the number of specifiers.
    fstr = Scanf::FormatString.new(str)
    last_spec = fstr.last_spec
    begin
      current = scanf(str)
      break if current.empty?
      final.push(yield(current))
    end until eof || fstr.last_spec_tried == last_spec
    return final
  end
end

class String

  # :section: scanf
  #
  # You will need to require 'scanf' to use these methods

  # Scans the current string. If a block is given, it
  # functions exactly like block_scanf.
  #
  #   arr = "123 456".scanf("%d%d")
  #   # => [123, 456]
  #
  #   require 'pp'
  #
  #   "this 123 read that 456 other".scanf("%s%d%s") {|m| pp m}
  #
  #   # ["this", 123, "read"]
  #   # ["that", 456, "other"]
  #   # => [["this", 123, "read"], ["that", 456, "other"]]
  #
  # See Scanf for details on creating a format string.
  #
  # You will need to require 'scanf' to use String#scanf
  def scanf(fstr,&b) #:yield: current_match
    if b
      block_scanf(fstr,&b)
    else
      fs =
        if fstr.is_a? Scanf::FormatString
          fstr
        else
          Scanf::FormatString.new(fstr)
        end
      fs.match(self)
    end
  end

  # Scans the current string until the match is exhausted
  # yielding each match as it is encountered in the string.
  # A block is not necessary as the results will simply
  # be aggregated into the final array.
  #
  #   "123 456".block_scanf("%d")
  #   # => [123, 456]
  #
  # If a block is given, the value from that is returned from
  # the yield is added to an output array.
  #
  #   "123 456".block_scanf("%d) do |digit,| # the ',' unpacks the Array
  #     digit + 100
  #   end
  #   # => [223, 556]
  #
  # See Scanf for details on creating a format string.
  #
  # You will need to require 'scanf' to use String#block_scanf
  def block_scanf(fstr) #:yield: current_match
    fs = Scanf::FormatString.new(fstr)
    str = self.dup
    final = []
    begin
      current = str.scanf(fs)
      final.push(yield(current)) unless current.empty?
      str = fs.string_left
    end until current.empty? || str.empty?
    return final
  end
end

module Kernel
  private
  # Scans STDIN for data matching +format+.  See IO#scanf for details.
  #
  # See Scanf for details on creating a format string.
  #
  # You will need to require 'scanf' to use Kernel#scanf.
  def scanf(format, &b) #:doc:
    STDIN.scanf(format ,&b)
  end
end
