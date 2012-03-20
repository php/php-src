require 'prettyprint'
require 'test/unit'

module PrettyPrintTest

class WadlerExample < Test::Unit::TestCase # :nodoc:
  def setup
    @tree = Tree.new("aaaa", Tree.new("bbbbb", Tree.new("ccc"),
                                               Tree.new("dd")),
                             Tree.new("eee"),
                             Tree.new("ffff", Tree.new("gg"),
                                              Tree.new("hhh"),
                                              Tree.new("ii")))
  end

  def hello(width)
    PrettyPrint.format('', width) {|hello|
      hello.group {
        hello.group {
          hello.group {
            hello.group {
              hello.text 'hello'
              hello.breakable; hello.text 'a'
            }
            hello.breakable; hello.text 'b'
          }
          hello.breakable; hello.text 'c'
        }
        hello.breakable; hello.text 'd'
      }
    }
  end

  def test_hello_00_06
    expected = <<'End'.chomp
hello
a
b
c
d
End
    assert_equal(expected, hello(0))
    assert_equal(expected, hello(6))
  end

  def test_hello_07_08
    expected = <<'End'.chomp
hello a
b
c
d
End
    assert_equal(expected, hello(7))
    assert_equal(expected, hello(8))
  end

  def test_hello_09_10
    expected = <<'End'.chomp
hello a b
c
d
End
    out = hello(9); assert_equal(expected, out)
    out = hello(10); assert_equal(expected, out)
  end

  def test_hello_11_12
    expected = <<'End'.chomp
hello a b c
d
End
    assert_equal(expected, hello(11))
    assert_equal(expected, hello(12))
  end

  def test_hello_13
    expected = <<'End'.chomp
hello a b c d
End
    assert_equal(expected, hello(13))
  end

  def tree(width)
    PrettyPrint.format('', width) {|q| @tree.show(q)}
  end

  def test_tree_00_19
    expected = <<'End'.chomp
aaaa[bbbbb[ccc,
           dd],
     eee,
     ffff[gg,
          hhh,
          ii]]
End
    assert_equal(expected, tree(0))
    assert_equal(expected, tree(19))
  end

  def test_tree_20_22
    expected = <<'End'.chomp
aaaa[bbbbb[ccc, dd],
     eee,
     ffff[gg,
          hhh,
          ii]]
End
    assert_equal(expected, tree(20))
    assert_equal(expected, tree(22))
  end

  def test_tree_23_43
    expected = <<'End'.chomp
aaaa[bbbbb[ccc, dd],
     eee,
     ffff[gg, hhh, ii]]
End
    assert_equal(expected, tree(23))
    assert_equal(expected, tree(43))
  end

  def test_tree_44
    assert_equal(<<'End'.chomp, tree(44))
aaaa[bbbbb[ccc, dd], eee, ffff[gg, hhh, ii]]
End
  end

  def tree_alt(width)
    PrettyPrint.format('', width) {|q| @tree.altshow(q)}
  end

  def test_tree_alt_00_18
    expected = <<'End'.chomp
aaaa[
  bbbbb[
    ccc,
    dd
  ],
  eee,
  ffff[
    gg,
    hhh,
    ii
  ]
]
End
    assert_equal(expected, tree_alt(0))
    assert_equal(expected, tree_alt(18))
  end

  def test_tree_alt_19_20
    expected = <<'End'.chomp
aaaa[
  bbbbb[ ccc, dd ],
  eee,
  ffff[
    gg,
    hhh,
    ii
  ]
]
End
    assert_equal(expected, tree_alt(19))
    assert_equal(expected, tree_alt(20))
  end

  def test_tree_alt_20_49
    expected = <<'End'.chomp
aaaa[
  bbbbb[ ccc, dd ],
  eee,
  ffff[ gg, hhh, ii ]
]
End
    assert_equal(expected, tree_alt(21))
    assert_equal(expected, tree_alt(49))
  end

  def test_tree_alt_50
    expected = <<'End'.chomp
aaaa[ bbbbb[ ccc, dd ], eee, ffff[ gg, hhh, ii ] ]
End
    assert_equal(expected, tree_alt(50))
  end

  class Tree # :nodoc:
    def initialize(string, *children)
      @string = string
      @children = children
    end

    def show(q)
      q.group {
        q.text @string
        q.nest(@string.length) {
          unless @children.empty?
            q.text '['
            q.nest(1) {
              first = true
              @children.each {|t|
                if first
                  first = false
                else
                  q.text ','
                  q.breakable
                end
                t.show(q)
              }
            }
            q.text ']'
          end
        }
      }
    end

    def altshow(q)
      q.group {
        q.text @string
        unless @children.empty?
          q.text '['
          q.nest(2) {
            q.breakable
            first = true
            @children.each {|t|
              if first
                first = false
              else
                q.text ','
                q.breakable
              end
              t.altshow(q)
            }
          }
          q.breakable
          q.text ']'
        end
      }
    end

  end
end

class StrictPrettyExample < Test::Unit::TestCase # :nodoc:
  def prog(width)
    PrettyPrint.format('', width) {|q|
      q.group {
        q.group {q.nest(2) {
                     q.text "if"; q.breakable;
                     q.group {
                       q.nest(2) {
                         q.group {q.text "a"; q.breakable; q.text "=="}
                         q.breakable; q.text "b"}}}}
        q.breakable
        q.group {q.nest(2) {
                     q.text "then"; q.breakable;
                     q.group {
                       q.nest(2) {
                         q.group {q.text "a"; q.breakable; q.text "<<"}
                         q.breakable; q.text "2"}}}}
        q.breakable
        q.group {q.nest(2) {
                     q.text "else"; q.breakable;
                     q.group {
                       q.nest(2) {
                         q.group {q.text "a"; q.breakable; q.text "+"}
                         q.breakable; q.text "b"}}}}}
    }
  end

  def test_00_04
    expected = <<'End'.chomp
if
  a
    ==
    b
then
  a
    <<
    2
else
  a
    +
    b
End
    assert_equal(expected, prog(0))
    assert_equal(expected, prog(4))
  end

  def test_05
    expected = <<'End'.chomp
if
  a
    ==
    b
then
  a
    <<
    2
else
  a +
    b
End
    assert_equal(expected, prog(5))
  end

  def test_06
    expected = <<'End'.chomp
if
  a ==
    b
then
  a <<
    2
else
  a +
    b
End
    assert_equal(expected, prog(6))
  end

  def test_07
    expected = <<'End'.chomp
if
  a ==
    b
then
  a <<
    2
else
  a + b
End
    assert_equal(expected, prog(7))
  end

  def test_08
    expected = <<'End'.chomp
if
  a == b
then
  a << 2
else
  a + b
End
    assert_equal(expected, prog(8))
  end

  def test_09
    expected = <<'End'.chomp
if a == b
then
  a << 2
else
  a + b
End
    assert_equal(expected, prog(9))
  end

  def test_10
    expected = <<'End'.chomp
if a == b
then
  a << 2
else a + b
End
    assert_equal(expected, prog(10))
  end

  def test_11_31
    expected = <<'End'.chomp
if a == b
then a << 2
else a + b
End
    assert_equal(expected, prog(11))
    assert_equal(expected, prog(15))
    assert_equal(expected, prog(31))
  end

  def test_32
    expected = <<'End'.chomp
if a == b then a << 2 else a + b
End
    assert_equal(expected, prog(32))
  end

end

class TailGroup < Test::Unit::TestCase # :nodoc:
  def test_1
    out = PrettyPrint.format('', 10) {|q|
      q.group {
        q.group {
          q.text "abc"
          q.breakable
          q.text "def"
        }
        q.group {
          q.text "ghi"
          q.breakable
          q.text "jkl"
        }
      }
    }
    assert_equal("abc defghi\njkl", out)
  end
end

class NonString < Test::Unit::TestCase # :nodoc:
  def format(width)
    PrettyPrint.format([], width, 'newline', lambda {|n| "#{n} spaces"}) {|q|
      q.text(3, 3)
      q.breakable(1, 1)
      q.text(3, 3)
    }
  end

  def test_6
    assert_equal([3, "newline", "0 spaces", 3], format(6))
  end

  def test_7
    assert_equal([3, 1, 3], format(7))
  end

end

class Fill < Test::Unit::TestCase # :nodoc:
  def format(width)
    PrettyPrint.format('', width) {|q|
      q.group {
        q.text 'abc'
        q.fill_breakable
        q.text 'def'
        q.fill_breakable
        q.text 'ghi'
        q.fill_breakable
        q.text 'jkl'
        q.fill_breakable
        q.text 'mno'
        q.fill_breakable
        q.text 'pqr'
        q.fill_breakable
        q.text 'stu'
      }
    }
  end

  def test_00_06
    expected = <<'End'.chomp
abc
def
ghi
jkl
mno
pqr
stu
End
    assert_equal(expected, format(0))
    assert_equal(expected, format(6))
  end

  def test_07_10
    expected = <<'End'.chomp
abc def
ghi jkl
mno pqr
stu
End
    assert_equal(expected, format(7))
    assert_equal(expected, format(10))
  end

  def test_11_14
    expected = <<'End'.chomp
abc def ghi
jkl mno pqr
stu
End
    assert_equal(expected, format(11))
    assert_equal(expected, format(14))
  end

  def test_15_18
    expected = <<'End'.chomp
abc def ghi jkl
mno pqr stu
End
    assert_equal(expected, format(15))
    assert_equal(expected, format(18))
  end

  def test_19_22
    expected = <<'End'.chomp
abc def ghi jkl mno
pqr stu
End
    assert_equal(expected, format(19))
    assert_equal(expected, format(22))
  end

  def test_23_26
    expected = <<'End'.chomp
abc def ghi jkl mno pqr
stu
End
    assert_equal(expected, format(23))
    assert_equal(expected, format(26))
  end

  def test_27
    expected = <<'End'.chomp
abc def ghi jkl mno pqr stu
End
    assert_equal(expected, format(27))
  end

end

end
