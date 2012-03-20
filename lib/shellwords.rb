##
# = Manipulates strings like the UNIX Bourne shell
#
# This module manipulates strings according to the word parsing rules
# of the UNIX Bourne shell.
#
# The shellwords() function was originally a port of shellwords.pl,
# but modified to conform to POSIX / SUSv3 (IEEE Std 1003.1-2001).
#
# == Example
#
#   argv = Shellwords.split('here are "two words"') # or String#shellsplit
#   argv #=> ["here", "are", "two words"]
#
#   argv = Shellwords.escape("special's.txt") # or String#shellescape
#   system("cat " + argv)
#
# == Authors:
# * Wakou Aoyama
# * Akinori MUSHA <knu@iDaemons.org>
#
# == Contact:
# * Akinori MUSHA <knu@iDaemons.org> (current maintainer)

module Shellwords
  # Splits a string into an array of tokens in the same way the UNIX
  # Bourne shell does.
  #
  #   argv = Shellwords.split('here are "two words"')
  #   argv #=> ["here", "are", "two words"]
  #
  # String#shellsplit is a shorthand for this function.
  #
  #   argv = 'here are "two words"'.shellsplit
  #   argv #=> ["here", "are", "two words"]
  def shellsplit(line)
    words = []
    field = ''
    line.scan(/\G\s*(?>([^\s\\\'\"]+)|'([^\']*)'|"((?:[^\"\\]|\\.)*)"|(\\.?)|(\S))(\s|\z)?/m) do
      |word, sq, dq, esc, garbage, sep|
      raise ArgumentError, "Unmatched double quote: #{line.inspect}" if garbage
      field << (word || sq || (dq || esc).gsub(/\\(.)/, '\\1'))
      if sep
        words << field
        field = ''
      end
    end
    words
  end

  alias shellwords shellsplit

  module_function :shellsplit, :shellwords

  class << self
    alias split shellsplit
  end

  # Escapes a string so that it can be safely used in a Bourne shell
  # command line.  +str+ can be a non-string object that responds to
  # +to_s+.
  #
  # Note that a resulted string should be used unquoted and is not
  # intended for use in double quotes nor in single quotes.
  #
  #   open("| grep #{Shellwords.escape(pattern)} file") { |pipe|
  #     # ...
  #   }
  #
  # String#shellescape is a shorthand for this function.
  #
  #   open("| grep #{pattern.shellescape} file") { |pipe|
  #     # ...
  #   }
  #
  # It is caller's responsibility to encode the string in the right
  # encoding for the shell environment where this string is used.
  # Multibyte characters are treated as multibyte characters, not
  # bytes.
  def shellescape(str)
    str = str.to_s

    # An empty argument will be skipped, so return empty quotes.
    return "''" if str.empty?

    str = str.dup

    # Treat multibyte characters as is.  It is caller's responsibility
    # to encode the string in the right encoding for the shell
    # environment.
    str.gsub!(/([^A-Za-z0-9_\-.,:\/@\n])/, "\\\\\\1")

    # A LF cannot be escaped with a backslash because a backslash + LF
    # combo is regarded as line continuation and simply ignored.
    str.gsub!(/\n/, "'\n'")

    return str
  end

  module_function :shellescape

  class << self
    alias escape shellescape
  end

  # Builds a command line string from an argument list +array+ joining
  # all elements escaped for Bourne shell into a single string with
  # fields separated by a space, where each element is stringified
  # using +to_s+.
  #
  #   open('|' + Shellwords.join(['grep', pattern, *files])) { |pipe|
  #     # ...
  #   }
  #
  # Array#shelljoin is a shorthand for this function.
  #
  #   open('|' + ['grep', pattern, *files].shelljoin) { |pipe|
  #     # ...
  #   }
  #
  # It is allowed to mix non-string objects in the elements as allowed
  # in Array#join.
  #
  #   output = `#{['ps', '-p', $$].shelljoin}`
  #
  def shelljoin(array)
    array.map { |arg| shellescape(arg) }.join(' ')
  end

  module_function :shelljoin

  class << self
    alias join shelljoin
  end
end

class String
  # call-seq:
  #   str.shellsplit => array
  #
  # Splits +str+ into an array of tokens in the same way the UNIX
  # Bourne shell does.  See Shellwords::shellsplit for details.
  def shellsplit
    Shellwords.split(self)
  end

  # call-seq:
  #   str.shellescape => string
  #
  # Escapes +str+ so that it can be safely used in a Bourne shell
  # command line.  See Shellwords::shellescape for details.
  def shellescape
    Shellwords.escape(self)
  end
end

class Array
  # call-seq:
  #   array.shelljoin => string
  #
  # Builds a command line string from an argument list +array+ joining
  # all elements escaped for Bourne shell and separated by a space.
  # See Shellwords::shelljoin for details.
  def shelljoin
    Shellwords.join(self)
  end
end
