# $Id$
#
# scanf for Ruby
#
# Unit tests
#

require 'scanf.rb'
require 'test/unit'
require 'tmpdir'

# Comment out either of these lines to skip those tests.

class TestStringScanf < Test::Unit::TestCase;end
class TestIOScanf < Test::Unit::TestCase;end

module ScanfTests

  def tests
    [

# Scratchpad
      [ "%2[a]", "nbc", []],
      [ "%*d %*3d %*s", "123 +456 abc", [] ],
      [ "%d%c", "123 x", [ 123, " " ] ],
      [ "%d%c", "123x", [ 123, "x" ] ],
      [ "%d %c", "123x", [ 123, "x" ] ],
      [ "%d %c", "123 x", [ 123, "x" ] ],

# Testing failures
      [ "%x", "x", [] ],
      [ "%2x", "x", [] ],
      [ "%i", "x", [] ],
#  ]; end; def nothing; [
      [ "%2i", "x", [] ],
      [ "%2o", "x", [] ],
      [ "%d", "x", [] ],
      [ "%2d", "x", [] ],
      [ "%3d", "+x3", [] ],
      [ "%d%[abc]", "eabc", [] ],
      [ "%d\n%[abc]", "\neabc", [] ],
      [ "%d%[^abc]", "ghiabc", [ ] ],
      [ "%d%[abc]", "abc", [] ],
      [ "%*d %*3d %*s", "123 +456 abc", [] ],
      [ "%d%s", "", [] ],
      [ "%d%s", "blah 123 string", [] ],
      [ "%[\n]", "abc\n", [] ],
      [ "%[\n]", "abc\n", [] ],
      [ "%[\n]", "abc\n", [] ],
      [ "%f", "x", [] ],
      [ "%f", "z", [] ],
      [ "%f", "z3.2534", [] ],
      [ "", "", [] ],
      [ "", "abc 123", [] ],
      [ '%[^\\w]%c', "a...1", [] ],

# Testing 'x'
      [ "%3x", "0xz", [0] ],

# Testing 'i'
      [ "%3i", "097", [0] ],
      [ "%3i", "0xz", [0] ],
      [ "%1i", "3", [ 3 ] ],
      [ "%2i", "07", [ 7 ] ],
      [ "%2i", "0a", [ 0 ] ],

# Testing 'c'
      [ "%3c", "abc", [ "abc" ] ],
      [ "%3c", "a\nb", [ "a\nb" ] ],
      [ "%3c", "a\nbcd", [ "a\nb" ] ],
      [ "%c\n\n", "x\n\n", [ "x" ] ],
      [ "%c\n\n", "x\n\n", [ "x" ] ],
      [ "%c", "\n", [ "\n" ] ],
      [ "%c", "x\n", [ "x" ] ],
      [ "%2c", " 123", [" 1"] ],
      [ " %c", " x", ["x"] ],
      [ "%c", " x", [" "] ],
      [ "%c", "123", ["1"] ],
      [ "%2c", "123", ["12"] ],
      [ "%5c", "a\nb\n\n", [ "a\nb\n\n" ] ],
      [ "%6c", "a\nb\n\nx", [ "a\nb\n\nx" ] ],
      [ "%5c", "ab\ncd", [ "ab\ncd" ] ],
      [ "%5c", "a\nb\n\n", [ "a\nb\n\n" ] ],

# Testing 'o'
      [ "%3o", "0xz", [0] ],

# Testing 'd'
      [ "%d", "\n123", [ 123 ] ],
      [ "%d", "\n\n123", [ 123 ] ],
      [ "%d", "\n123", [ 123 ] ],
      [ "%1d", "2", [2] ],

# Mixed tests
# Includes:
#   whitespace/newline
#   mixed integer bases
#   various mixed specifiers

      [ "%[^\\w]%c", "...1", [ "...", "1"] ],
      [ '%[^\\w]%c', "...1", [ "...", "1"] ],
      [ "%[abc\n]%d", "a\n\nb\n\nc   123", [ "a\n\nb\n\nc", 123 ] ],
      [ "%[abc\n]%d", "a\n\nb\n\nc \t  123", [ "a\n\nb\n\nc", 123 ] ],
      [ "%[abc\t]%d", "a\t\tb\t\tc   123", [ "a\t\tb\t\tc", 123 ] ],
      [ "%d%3[abc\n]", "123a\nbeaab", [ 123, "a\nb" ] ],
      [ "%d%20c",  "42   is the key", [ 42, "   is the key" ] ],
      [ "%d %20c",  "42   is the key", [ 42, "is the key" ] ],
      [ "%d%3[^abc\n]%d", "123de\nf123", [ 123, "de" ] ],
      [ "%d %4c", "3abc", [ 3, "abc" ] ],
      [ "%f%d\n%[abc]", "1\neabc", [1.0] ],
      [ "%d%3[abc]", "123aaab", [ 123, "aaa" ] ],
      [ "%d%3[abc]", "123 aaab", [ 123 ] ],
      [ "%d%3[abc]", "123aeaab", [ 123, "a" ] ],
      [ "%d%[^abc]", "123defabc", [123, "def" ] ],
      [ "%d%3[^abc]", "123defdef", [ 123, "def" ] ],
      [ "%d%3[^abc] ", "123defdef   ", [ 123, "def" ] ],
      [ "%d%3[^abc]ghi", "123defghi", [ 123, "def" ] ],
      [ "%d%3[^abc]", "123defdef", [ 123, "def" ] ],
      [ "%d%3[^abc]", "123adefdef", [ 123 ] ],
      [ "%d%3[^abc]", "123deafdef", [ 123, "de" ] ],
      [ "%d%3[^abc\n]", "123de\nf", [ 123, "de" ] ],
      [ "%s%c%c%s", "abc\n\ndef", ["abc", "\n","\n", "def" ] ],
      [ "%c%d", "\n\n123", [ "\n",123 ] ],
      [ "%s%c%d", "abc\n123", [ "abc", "\n", 123 ] ],
      [ "%s%c%d", "abc\n\n123", [ "abc", "\n", 123 ] ],
      [ "%c%d", "\t\n123", [ "\t",123 ] ],
      [ "%s%c%d", "abc\t\n123", [ "abc", "\t", 123 ] ],
      [ "%3c%d", "abc123", [ "abc", 123 ] ],
      [ "%3c\n%d", "abc123", [ "abc", 123 ] ],
      [ "%3c\n%d", "abc 123", [ "abc", 123 ] ],
      [ "%3c %d", "abc123", [ "abc", 123 ] ],
      [ "%3c\t%d", "abc \n 123", [ "abc", 123 ] ],
      [ "%3c\t%d", "abc \n 123   ", [ "abc", 123 ] ],
      [ "%3c%d", "a\nb123", [ "a\nb", 123 ] ],
      [ "%f%3c", "1.2x\ny", [ 1.2, "x\ny"] ],
      [ "%d\n%d\n%d", "123 456 789", [ 123,456,789 ] ],
      [ "%d\n%i%2d%x\n%d", "123 0718932", [ 123, 071, 89, 0x32] ],
      [ "%c\n%c", "x y", [ "x", "y" ] ],
      [ "%c\t%c", "x y", [ "x", "y" ] ],
      [ "%s\n%s", "x y", [ "x", "y" ] ],
      [ "%s%s\n", "x y", [ "x", "y" ] ],
      [ "%c\n\n%c", "x\n\ny", [ "x", "y" ] ],
      [ "%s%d%d", "abc\n123\n456", [ "abc", 123, 456 ] ],
      [ "%3s%c%3c%d", "1.2x\n\ny123", [ "1.2", "x", "\n\ny", 123 ] ],
      [ "%f%3c", "1.2x\ny", [ 1.2, "x\ny"] ],
      [ "%c\n%c", "x\n\ny", [ "x", "y" ] ],
      [ "%c\n\n%c", "x\n\ny", [ "x", "y" ] ],
      [ "%c  %c", "x\n\ny", [ "x", "y" ] ],
      [ "%s\n\n%c", "x\n\ny", [ "x", "y" ] ],
      [ "%s\n\n%s", "x\n\ny", [ "x", "y" ] ],
      [ "%d\n\n%d", "23\n\n45", [ 23, 45 ] ],
      [ "%d\n%d", "23\n\n45", [ 23, 45 ] ],
      [ "%c\n\n%c", "x y", [ "x", "y" ] ],
      [ "%c%c", "x\n\ny", [ "x", "\n" ] ],
      [ "%c\n%c", "x y", [ "x", "y" ] ],
      [ "%c\t%c", "x y", [ "x", "y" ] ],
      [ "%s\n%s", "x y", [ "x", "y" ] ],
      [ "%s%s\n", "x y", [ "x", "y" ] ],
      [ "%c%c", "x\n", [ "x", "\n" ] ],
      [ "%d%c%c%d", "345   678", [ 345, " ", " ", 678] ],
      [ "%d   %c%s", "123   x  hello", [123, "x", "hello"] ],
      [ "%d%2c", "654 123", [654," 1"] ],
      [ "%5c%s", "a\nb\n\nxyz", [ "a\nb\n\n","xyz" ] ],
      [ "%s%[ xyz]%d", "hello x 32", ["hello", " x ", 32] ],
      [ "%5s%8[a-z]%d", "helloblahblah 32", ["hello", "blahblah", 32] ],
      [ '%s%[abcde\\s]%d', "hello badea 32", ["hello", " badea ", 32] ],
      [ '%d%[\\s]%c', "123 \n\t X", [ 123," \n\t ", "X"] ],
      [ "%4s%2c%c", "1.2x\n\ny", [ "1.2x", "\n\n","y"] ],
      [ "%f%c %3c%d", "1.2x\n\ny123", [ 1.2, "x", "y12", 3 ] ],
      [ "%s%5c", "abc ab\ncd", [ "abc", " ab\nc" ] ],
      [ "%5c%f", "ab\ncd1.2", [ "ab\ncd",1.2 ] ],
      [ "%5c%c", "ab\ncd1", [ "ab\ncd","1" ] ],
      [ "%f%c%2c%d", "1.2x\ny123", [ 1.2, "x", "\ny", 123 ] ],
      [ "%f%c%3c", "1.2x\ny123", [ 1.2, "x", "\ny1"] ],
      [ "%s\n%s", "blah\n\nand\nmore stuff", [ "blah", "and" ] ],
      [ "%o%d%x", "21912a3", [ "21".oct, 912, "a3".hex ] ],
      [ "%3o%4d%3x", "21912a3", [ "21".oct, 912, "a3".hex ] ],
      [ "%3o%4d%5x", "2191240xa3", [ "21".oct, 9124, "a3".hex ] ],
      [ "%3d%3x", "12abc", [12, "abc".hex] ],
      [ "%s%i%d", "hello +0xdef 123", [ "hello", "def".hex, 123] ],
      [ "%s%i%d", "hello -0xdef 123", [ "hello", -"def".hex, 123] ],
      [ "%s%i%i%i%i", "hello 012 -012 100 1", [ "hello", 10, -10, 100, 1 ] ],
      [ "%s%i%i%i%i", "hello 012 0x12 100 1", [ "hello", 10, 18, 100, 1 ] ],
      [ "%s%5i%3i%4i", "hello 0x123 123 0123", [ "hello", "0x123".hex, 123,"0123".oct] ],
      [ "%s%3i%4i", "hello 1230123", [ "hello", 123,"0123".oct] ],
      [ "%s%3i", "hello 1230", [ "hello", 123] ],
      [ "%s%5x%d", "hello 0xdef 123", [ "hello", "def".hex, 123] ],
      [ "%s%6x%d", "hello +0xdef 123", [ "hello", "def".hex, 123] ],
      [ "%s%6x%d", "hello -0xdef 123", [ "hello", -"def".hex, 123] ],
      [ "%s%6x%d", "hello -0xdef 123", [ "hello", -"def".hex, 123] ],
      [ "%s%4x%d", "hello -def 123", [ "hello", -"def".hex, 123] ],
      [ "%s%3x%d", "hello def 123", [ "hello", "def".hex, 123] ],
      [ "%s%x%d", "hello -def 123", [ "hello", -"def".hex, 123] ],
      [ "%s%x%d", "hello -0xdef 123", [ "hello", -"def".hex, 123] ],
      [ "%s%x%d", "hello 0xdef 123", [ "hello", "def".hex, 123] ],
      [ "%s%d%x%s", "hello 123 abc def", [ "hello", 123, "abc".hex, "def"] ],
      [ "%s%d%o%d", "hello 012 012 100", [ "hello", 12, 10, 100 ] ],
      [ "%s%d%o%d", "hello 012 -012 100", [ "hello", 12, -10, 100 ] ],
      [ "%s%o%x%d", "hello 012 0x12 100", [ "hello", 10, 18, 100 ] ],
      [ "%s%d%o%d", "hello 012 +01288", [ "hello", 12, 10, 88 ] ],
      [ "%f %d %s", "12.3e23 45 string", ["12.3e23".to_f, 45, "string"] ],
      [ "%f %d %s", "12.3e+23 45 string", ["12.3e23".to_f, 45, "string"] ],
      [ "%f %d %s", "12.3e-23 45 string", ["12.3e-23".to_f, 45, "string"] ],
      [ "%f %d %s", "12.3e23 45 string", ["12.3e23".to_f, 45, "string"] ],
      [ "%f %d %s", "-12.3e-23 45 string", ["-12.3e-23".to_f, 45, "string"] ],
      [ "%f %d %s", "12.e23 45 string", ["12.e23".to_f, 45, "string"] ],
      [ "%5f %d %s", "1.2e23 string", ["1.2e2".to_f, 3, "string"] ],
      [ "%5f%d %s", "1.2e23 string", ["1.2e2".to_f, 3, "string"] ],
      [ "%5f%d %d %s", "1.2e23 45 string", ["1.2e2".to_f, 3, 45, "string"] ],
      [ "%6f %d %d %s", "+1.2e23 45 string", ["1.2e2".to_f, 3, 45, "string"] ],
      [ "%d %d", "123 \n 345", [123, 345] ],
      [ "%d %*d", "123 \n 345", [123] ],
      [ "%d %3d789", "123 +45789", [123, 45] ],
      [ "%d %3d%d", "123 +456789", [123, 45, 6789] ],
      [ "%d %3dabc", "123 456abc", [123, 456] ],
      [ "%d %s", "123abc", [123, "abc"] ],
      [ "%d%s %s", "123 abc def", [123, "abc", "def"] ],
      [ "%s%s", "abc123 def", ["abc123", "def"] ],
      [ "%s%s %s", "123 abc def", ["123", "abc", "def"] ],
      [ "%s%%%s", "abc % def", ["abc", "def"] ],
      [ "%d %3d %s", "+123 456abc", [123, 456, "abc"] ],
      [ "%d %3d %s", "123 456abc", [123, 456, "abc"] ],
      [ "%d %3d %s", "123 +456 abc", [123, 45, "6"] ],
      [ "%d %3d %s", "-123-456abc", [-123, -45, "6abc"] ],
      [ "%dabc%d", "123abc345", [123, 345] ],
      [ "%d%5s%d", "123 abcde12", [123, "abcde", 12] ],
      [ "%5d%5s%5d", "12345abcde67890", [12345, "abcde", 67890] ],
      [ "%5d%*5s%5d", "12345abcde67890", [12345, 67890] ],
      [ " 12345%5s%5d", "12345abcde67890", [ "abcde", 67890] ],
      [ "%5dabcde%5d", "12345abcde67890", [ 12345, 67890] ],
      [ "%s%%%*s", "abc % def", ["abc"] ],
      [ "%*6s %d", "string 123", [123] ],
      [ "%d %*3d %s", "-123-456abc", [-123, "6abc"] ],
      [ "%d%s", "123", [123] ],
      [ "%s%d", "abc", ["abc"] ],
      [ "%f%x", "3.2e45x", ["3.2e45x".to_f] ],
      [ "%s%d", "abc", ["abc"] ],
      [ "%*5f%d %d %s", "1.2e23 45 string", [3, 45, "string"] ],
      [ "%5f%*d %d %s", "1.2e23 45 string", ["1.2e2".to_f, 45, "string"] ],
      [ "%*5f%*d %*d %s", "1.2e23 45 string", ["string"] ],
      [ "%f %*d %s", "12.e23 45 string", ["12.e23".to_f, "string"] ],
      [ "%5f %d %s", "1.2e23 string", ["1.2e2".to_f, 3, "string"] ],
      [ "%s %f %s %d %x%c%c%c%c",
        "float: 1.2e23 dec/hex: 135a23 abc",
        ["float:", "1.2e23".to_f, "dec/hex:", 135, "a23".hex, " ", "a", "b", "c" ] ],

# Testing 's'
      [ "%s\n", "blah\n\n\n", [ "blah" ] ],

# Testing '['
      [ "%[a\nb]", "a\nb", [ "a\nb" ] ],
      [ "%[abc]", "acb", [ "acb" ] ],
      [ "%[abc\n]", "a\nb", [ "a\nb" ] ],
      [ "%[^abc]", "defabc", [ "def" ] ],
      [ "%[-abc]", "abc-cba", [ "abc-cba" ] ],
      [ "%[\n]", "\n", [ "\n" ] ],
      [ "%[\n]", "\nabc", [ "\n" ] ],
      [ "%[\n\t]", "\t\n", [ "\t\n" ] ],
      [ "%[a-f]", "abczef", [ "abc" ] ],
      [ "%d%3[abc]", "123 aaab", [ 123 ] ],
      [ "%d%3[^abc]", "123adefdef", [ 123 ] ],
      [ "%d%3[[:lower:]] %f", "123ade1.2", [ 123,"ade",1.2 ] ],
      [ "%d%3[[:lower:]] %f", "123ad1.2", [ 123,"ad",1.2 ] ],
      [ "%d%3[[:lower:]] %f", "123 ad1.2", [ 123 ] ],
      [ "%d%[[:lower:]]", "123abcdef1.2", [ 123, "abcdef" ] ],
      [ "%[[:lower:]]%d", "abcdef123", [ "abcdef", 123 ] ],
      [ "%[[:digit:]]%[[:alpha:]]", "123abcdef", [ "123", "abcdef" ] ],
      [ "%[[:digit:]]%d", "123 123", [ "123", 123 ] ],
      [ "%[[:upper:]]", "ABCdefGHI", [ "ABC" ] ],

# Testing 'f'
      [ "%2f", "x", [] ],
      [ "%F", "1.23e45", [1.23e+45] ],
      [ "%e", "3.25ee", [3.25] ],
      [ "%E", "3..25", [3.0] ],
      [ "%g", "+3.25", [3.25] ],
      [ "%G", "+3.25e2", [325.0] ],
      [ "%f", "3.z", [3.0] ],
      [ "%a", "0X1P+10", [1024.0] ],
      [ "%A", "0x1.deadbeefp+99", [1.1851510441583988e+30] ],

# Testing embedded matches including literal '[' behavior
      [",%d,%f", ",10,1.1", [10,1.1] ],
      [" ,%d,%f", " ,10,1.1", [10,1.1] ],
      ["[%d,%f", "[10,1.1", [10,1.1] ],
      [" [%d,%f", " [10,1.1", [10,1.1] ],

     ]
  end

  def each_test
    i = "0" * (Math.log(self.tests.size, 10).floor+1)
    self.tests.each do |test|
      yield test, i.succ!
    end
  end
end

class TestStringScanf
  include Scanf
  extend ScanfTests

  self.each_test do |test, i|
    define_method("test_#{i}") do ||
      assert_equal(test[2], test[1].scanf(test[0]))
    end
  end
end

class TestIOScanf
  include Scanf
  extend ScanfTests

  tmpfilename = "#{Dir.tmpdir}/iotest.dat.#{$$}"

  self.each_test do |test, i|
    define_method("test_#{i}") do ||
      File.open(tmpfilename, "w") {|fh| fh.print test[1]}
      File.open(tmpfilename, "r") { |fh|
        assert_equal(test[2], fh.scanf(test[0]))
      }
      File.delete(tmpfilename)
    end
  end
end
