# coding: utf-8

require 'pp'
require 'rubygems'
require 'minitest/autorun'
require 'rdoc/markup'

class TestRDocMarkupParser < MiniTest::Unit::TestCase

  def setup
    @RM = RDoc::Markup
    @RMP = @RM::Parser
  end

  def mu_pp(obj)
    s = ''
    s = PP.pp obj, s
    s = s.force_encoding(Encoding.default_external) if defined? Encoding
    s.chomp
  end

  def test_build_heading
    parser = @RMP.new

    parser.tokens.replace [
      [:TEXT,    'heading three',  4, 0],
      [:NEWLINE, "\n",            17, 0],
    ]

    assert_equal @RM::Heading.new(3, 'heading three'), parser.build_heading(3)
  end

  def test_get
    parser = util_parser

    assert_equal [:HEADER, 1, 0, 0], parser.get

    assert_equal 7, parser.tokens.length
  end

  def test_parse_bullet
    str = <<-STR
* l1
* l2
    STR

    expected = [
      @RM::List.new(:BULLET, *[
        @RM::ListItem.new(nil,
          @RM::Paragraph.new('l1')),
        @RM::ListItem.new(nil,
          @RM::Paragraph.new('l2'))])]

    assert_equal expected, @RMP.parse(str).parts
  end

  def test_parse_bullet_utf_8
    str = <<-STR
* 新しい機能
    STR

    expected = [
      @RM::List.new(:BULLET, *[
        @RM::ListItem.new(nil,
          @RM::Paragraph.new('新しい機能'))])]

    assert_equal expected, @RMP.parse(str).parts
  end

  def test_parse_bullet_verbatim_heading
    str = <<-STR
* l1
    v

= H
    STR

    expected = [
      @RM::List.new(:BULLET, *[
        @RM::ListItem.new(nil,
          @RM::Paragraph.new('l1'),
          @RM::Verbatim.new("v\n"))]),
      @RM::Heading.new(1, 'H')]

    assert_equal expected, @RMP.parse(str).parts
  end

  def test_parse_bullet_heading
    str = <<-STR
* = l1
    STR

    expected = [
      @RM::List.new(:BULLET, *[
        @RM::ListItem.new(nil,
          @RM::Heading.new(1, 'l1'))])]

    assert_equal expected, @RMP.parse(str).parts
  end

  def test_parse_bullet_indent
    str = <<-STR
* l1
  * l1.1
* l2
    STR

    expected = [
      @RM::List.new(:BULLET, *[
        @RM::ListItem.new(nil,
          @RM::Paragraph.new('l1'),
          @RM::List.new(:BULLET, *[
            @RM::ListItem.new(nil,
              @RM::Paragraph.new('l1.1'))])),
        @RM::ListItem.new(nil,
          @RM::Paragraph.new('l2'))])]

    assert_equal expected, @RMP.parse(str).parts
  end

  def test_parse_bullet_paragraph
    str = <<-STR
now is
* l1
* l2
the time
    STR

    expected = [
      @RM::Paragraph.new('now is'),
      @RM::List.new(:BULLET, *[
        @RM::ListItem.new(nil,
          @RM::Paragraph.new('l1')),
        @RM::ListItem.new(nil,
          @RM::Paragraph.new('l2')),
      ]),
      @RM::Paragraph.new('the time'),
    ]

    assert_equal expected, @RMP.parse(str).parts
  end

  def test_parse_bullet_multiline
    str = <<-STR
* l1
  l1+
* l2
    STR

    expected = [
      @RM::List.new(:BULLET, *[
        @RM::ListItem.new(nil,
          @RM::Paragraph.new('l1', 'l1+')),
        @RM::ListItem.new(nil,
          @RM::Paragraph.new('l2')),
      ]),
    ]

    assert_equal expected, @RMP.parse(str).parts
  end

  def test_parse_bullet_multiparagraph
    str = <<-STR
* l1

  l1+
    STR

    expected = [
      @RM::List.new(:BULLET, *[
        @RM::ListItem.new(nil,
          @RM::Paragraph.new('l1'),
          @RM::BlankLine.new,
          @RM::Paragraph.new('l1+')),
      ]),
    ]

    assert_equal expected, @RMP.parse(str).parts
  end

  def test_parse_bullet_indent_verbatim
    str = <<-STR
* l1
  * l1.1
    text
      code
        code

    text
* l2
    STR

    expected = [
      @RM::List.new(:BULLET, *[
        @RM::ListItem.new(nil,
          @RM::Paragraph.new('l1'),
          @RM::List.new(:BULLET, *[
            @RM::ListItem.new(nil,
              @RM::Paragraph.new('l1.1', 'text'),
              @RM::Verbatim.new("code\n", "  code\n"),
              @RM::Paragraph.new('text'))])),
        @RM::ListItem.new(nil,
          @RM::Paragraph.new('l2'))])]

    assert_equal expected, @RMP.parse(str).parts
  end

  def test_parse_dash
    str = <<-STR
- one
- two
    STR

    expected = [
      @RM::List.new(:BULLET, *[
        @RM::ListItem.new(nil,
          @RM::Paragraph.new('one')),
        @RM::ListItem.new(nil,
          @RM::Paragraph.new('two'))])]

    assert_equal expected, @RMP.parse(str).parts
  end

  def test_parse_heading
    str = '= heading one'

    expected = [
      @RM::Heading.new(1, 'heading one')]

    assert_equal expected, @RMP.parse(str).parts
  end

  def test_parse_heading_three
    str = '=== heading three'

    expected = [
      @RM::Heading.new(3, 'heading three')]

    assert_equal expected, @RMP.parse(str).parts
  end

  def test_parse_heading_bullet
    str = '= * heading one'

    expected = [
      @RM::Heading.new(1, '* heading one')]

    assert_equal expected, @RMP.parse(str).parts
  end

  def test_parse_heading_empty
    str = <<-STR
===
* bullet
    STR

    expected = [
      @RM::Heading.new(3, ''),
      @RM::BlankLine.new,
      @RM::List.new(:BULLET, *[
        @RM::ListItem.new(nil,
          @RM::Paragraph.new('bullet'))]),
    ]

    assert_equal expected, @RMP.parse(str).parts
  end

  def test_parse_heading_heading
    str = '= ='

    expected = [
      @RM::Heading.new(1, '=')]

    assert_equal expected, @RMP.parse(str).parts
  end

  def test_parse_heading_lalpha
    str = '= b. heading one'

    expected = [
      @RM::Heading.new(1, 'b. heading one')]

    assert_equal expected, @RMP.parse(str).parts
  end

  def test_parse_heading_label
    str = '= [heading one]'

    expected = [
      @RM::Heading.new(1, '[heading one]')]

    assert_equal expected, @RMP.parse(str).parts
  end

  def test_parse_heading_note
    str = '= heading one::'

    expected = [
      @RM::Heading.new(1, 'heading one::')]

    assert_equal expected, @RMP.parse(str).parts
  end

  def test_parse_heading_number
    str = '= 5. heading one'

    expected = [
      @RM::Heading.new(1, '5. heading one')]

    assert_equal expected, @RMP.parse(str).parts
  end

  def test_parse_heading_ualpha
    str = '= B. heading one'

    expected = [
      @RM::Heading.new(1, 'B. heading one')]

    assert_equal expected, @RMP.parse(str).parts
  end

  def test_parse_label
    str = <<-STR
[one] item one
[two] item two
    STR

    expected = [
      @RM::List.new(:LABEL, *[
        @RM::ListItem.new('one',
          @RM::Paragraph.new('item one')),
        @RM::ListItem.new('two',
          @RM::Paragraph.new('item two'))])]

    assert_equal expected, @RMP.parse(str).parts
  end

  def test_parse_label_bullet
    str = <<-STR
[cat] l1
      * l1.1
[dog] l2
    STR

    expected = [
      @RM::List.new(:LABEL, *[
        @RM::ListItem.new('cat',
          @RM::Paragraph.new('l1'),
          @RM::List.new(:BULLET, *[
            @RM::ListItem.new(nil,
              @RM::Paragraph.new('l1.1'))])),
        @RM::ListItem.new('dog',
          @RM::Paragraph.new('l2'))])]

    assert_equal expected, @RMP.parse(str).parts
  end

  def test_parse_label_multiline
    str = <<-STR
[cat] l1
      continuation
[dog] l2
    STR

    expected = [
      @RM::List.new(:LABEL, *[
        @RM::ListItem.new('cat',
          @RM::Paragraph.new('l1', 'continuation')),
        @RM::ListItem.new('dog',
          @RM::Paragraph.new('l2'))])]

    assert_equal expected, @RMP.parse(str).parts
  end

  def test_parse_label_newline
    str = <<-STR
[one]
  item one
[two]
  item two
    STR

    expected = [
      @RM::List.new(:LABEL, *[
        @RM::ListItem.new('one',
          @RM::Paragraph.new('item one')),
        @RM::ListItem.new('two',
          @RM::Paragraph.new('item two')),
    ])]

    assert_equal expected, @RMP.parse(str).parts
  end

  def test_parse_lalpha
    str = <<-STR
a. l1
b. l2
    STR

    expected = [
      @RM::List.new(:LALPHA, *[
        @RM::ListItem.new(nil,
          @RM::Paragraph.new('l1')),
        @RM::ListItem.new(nil,
          @RM::Paragraph.new('l2'))])]

    assert_equal expected, @RMP.parse(str).parts
  end

  def test_parse_lalpha_ualpha
    str = <<-STR
a. l1
b. l2
A. l3
A. l4
    STR

    expected = [
      @RM::List.new(:LALPHA, *[
        @RM::ListItem.new(nil,
          @RM::Paragraph.new('l1')),
        @RM::ListItem.new(nil,
          @RM::Paragraph.new('l2'))]),
      @RM::List.new(:UALPHA, *[
        @RM::ListItem.new(nil,
          @RM::Paragraph.new('l3')),
        @RM::ListItem.new(nil,
          @RM::Paragraph.new('l4'))])]

    assert_equal expected, @RMP.parse(str).parts
  end

  def test_parse_lalpha_utf_8
    str = <<-STR
a. 新しい機能
    STR

    expected = [
      @RM::List.new(:LALPHA, *[
        @RM::ListItem.new(nil,
          @RM::Paragraph.new('新しい機能'))])]

    assert_equal expected, @RMP.parse(str).parts
  end

  def test_parse_list_list_1
    str = <<-STR
10. para 1

    [label 1]
      para 1.1

        code

      para 1.2
    STR

    expected = [
      @RM::List.new(:NUMBER, *[
        @RM::ListItem.new(nil, *[
          @RM::Paragraph.new('para 1'),
          @RM::BlankLine.new,
          @RM::List.new(:LABEL, *[
            @RM::ListItem.new('label 1', *[
              @RM::Paragraph.new('para 1.1'),
              @RM::BlankLine.new,
              @RM::Verbatim.new("code\n"),
              @RM::Paragraph.new('para 1.2')])])])])]

    assert_equal expected, @RMP.parse(str).parts
  end

  def test_parse_list_list_2
    str = <<-STR
6. para

   label 1::  text 1
   label 2::  text 2
    STR

    expected = [
      @RM::List.new(:NUMBER, *[
        @RM::ListItem.new(nil, *[
          @RM::Paragraph.new('para'),
          @RM::BlankLine.new,
          @RM::List.new(:NOTE, *[
            @RM::ListItem.new('label 1',
              @RM::Paragraph.new('text 1')),
            @RM::ListItem.new('label 2',
              @RM::Paragraph.new('text 2'))])])])]

    assert_equal expected, @RMP.parse(str).parts
  end

  def test_parse_list_verbatim
    str = <<-STR
* one
    verb1
    verb2
* two
    STR

    expected = [
      @RM::List.new(:BULLET, *[
        @RM::ListItem.new(nil,
          @RM::Paragraph.new('one'),
          @RM::Verbatim.new("verb1\n", "verb2\n")),
        @RM::ListItem.new(nil,
          @RM::Paragraph.new('two'))])]

    assert_equal expected, @RMP.parse(str).parts
  end

  def test_parse_lists
    str = <<-STR
now is
* l1
1. n1
2. n2
* l2
the time
    STR

    expected = [
      @RM::Paragraph.new('now is'),
      @RM::List.new(:BULLET, *[
        @RM::ListItem.new(nil,
          @RM::Paragraph.new('l1'))]),
      @RM::List.new(:NUMBER, *[
        @RM::ListItem.new(nil,
          @RM::Paragraph.new('n1')),
        @RM::ListItem.new(nil,
          @RM::Paragraph.new('n2'))]),
      @RM::List.new(:BULLET, *[
        @RM::ListItem.new(nil,
          @RM::Paragraph.new('l2'))]),
      @RM::Paragraph.new('the time')]

    assert_equal expected, @RMP.parse(str).parts
  end

  def test_parse_note
    str = <<-STR
one:: item one
two:: item two
    STR

    expected = [
      @RM::List.new(:NOTE, *[
        @RM::ListItem.new('one',
          @RM::Paragraph.new('item one')),
        @RM::ListItem.new('two',
          @RM::Paragraph.new('item two'))])]

    assert_equal expected, @RMP.parse(str).parts
  end

  def test_parse_note_empty
    str = <<-STR
one::
two::
    STR

    expected = [
      @RM::List.new(:NOTE, *[
        @RM::ListItem.new('one',
          @RM::BlankLine.new),
        @RM::ListItem.new('two',
          @RM::BlankLine.new)])]

    assert_equal expected, @RMP.parse(str).parts
  end

  def test_parse_note_note
    str = <<-STR
one:: two::
    STR

    expected = [
      @RM::List.new(:NOTE, *[
        @RM::ListItem.new('one',
          @RM::List.new(:NOTE, *[
            @RM::ListItem.new('two',
              @RM::BlankLine.new)]))])]

    assert_equal expected, @RMP.parse(str).parts
  end

  def test_parse_number_bullet
    str = <<-STR
1. l1
   * l1.1
2. l2
    STR

    expected = [
      @RM::List.new(:NUMBER, *[
        @RM::ListItem.new(nil,
          @RM::Paragraph.new('l1'),
          @RM::List.new(:BULLET, *[
            @RM::ListItem.new(nil,
              @RM::Paragraph.new('l1.1'))])),
        @RM::ListItem.new(nil,
          @RM::Paragraph.new('l2'))])]

    assert_equal expected, @RMP.parse(str).parts
  end

  def test_parse_paragraph
    str = <<-STR
now is the time

for all good men
    STR

    expected = [
      @RM::Paragraph.new('now is the time'),
      @RM::BlankLine.new,
      @RM::Paragraph.new('for all good men')]
    assert_equal expected, @RMP.parse(str).parts
  end

  def test_parse_paragraph_multiline
    str = "now is the time\nfor all good men"

    expected = @RM::Paragraph.new 'now is the time for all good men'
    assert_equal [expected], @RMP.parse(str).parts
  end

  def test_parse_paragraph_verbatim
    str = <<-STR
now is the time
  code _line_ here
for all good men
    STR

    expected = [
      @RM::Paragraph.new('now is the time'),
      @RM::Verbatim.new("code _line_ here\n"),
      @RM::Paragraph.new('for all good men'),
    ]
    assert_equal expected, @RMP.parse(str).parts
  end

  def test_parse_rule
    str = <<-STR
now is the time

---

for all good men
    STR

    expected = [
      @RM::Paragraph.new('now is the time'),
      @RM::BlankLine.new,
      @RM::Rule.new(1),
      @RM::BlankLine.new,
      @RM::Paragraph.new('for all good men')]

    assert_equal expected, @RMP.parse(str).parts
  end

  def test_parse_ualpha
    str = <<-STR
A. l1
B. l2
    STR

    expected = [
      @RM::List.new(:UALPHA, *[
        @RM::ListItem.new(nil,
          @RM::Paragraph.new('l1')),
        @RM::ListItem.new(nil,
          @RM::Paragraph.new('l2'))])]

    assert_equal expected, @RMP.parse(str).parts
  end

  def test_parse_trailing_cr
    expected = [ @RM::Paragraph.new('Text') ]
    # FIXME hangs the parser:
    assert_equal expected, @RMP.parse("Text\r").parts
  end

  def test_parse_verbatim
    str = <<-STR
now is
   code
the time
    STR

    expected = [
      @RM::Paragraph.new('now is'),
      @RM::Verbatim.new("code\n"),
      @RM::Paragraph.new('the time'),
    ]

    assert_equal expected, @RMP.parse(str).parts
  end

  def test_parse_verbatim_bullet
    str = <<-STR
  * blah
    STR

    expected = [
      @RM::Verbatim.new("* blah\n")]

    assert_equal expected, @RMP.parse(str).parts
  end

  def test_parse_verbatim_dash
    str = <<-STR
  - blah
    STR

    expected = [
      @RM::Verbatim.new("- blah\n")]

    assert_equal expected, @RMP.parse(str).parts
  end

  def test_parse_verbatim_fold
    str = <<-STR
now is
   code


   code1

the time
    STR

    expected = [
      @RM::Paragraph.new('now is'),
      @RM::Verbatim.new("code\n", "\n", "code1\n"),
      @RM::Paragraph.new('the time'),
    ]

    assert_equal expected, @RMP.parse(str).parts
  end

  def test_parse_verbatim_heading
    str = <<-STR
text
   ===   heading three
    STR

    expected = [
      @RM::Paragraph.new('text'),
      @RM::Verbatim.new("===   heading three\n")]

    assert_equal expected, @RMP.parse(str).parts
  end

  def test_parse_verbatim_heading2
    str = "text\n   code\n=== heading three"

    expected = [
      @RM::Paragraph.new('text'),
      @RM::Verbatim.new("code\n"),
      @RM::Heading.new(3, 'heading three')]

    assert_equal expected, @RMP.parse(str).parts
  end

  def test_parse_verbatim_label
    str = <<-STR
  [blah] blah
    STR

    expected = [
      @RM::Verbatim.new("[blah] blah\n")]

    assert_equal expected, @RMP.parse(str).parts
  end

  def test_parse_verbatim_lalpha
    str = <<-STR
  b. blah
    STR

    expected = [
      @RM::Verbatim.new("b. blah\n")]

    assert_equal expected, @RMP.parse(str).parts
  end

  def test_parse_verbatim_markup_example
    str = <<-STR
text
   code
   === heading three
    STR

    expected = [
      @RM::Paragraph.new('text'),
      @RM::Verbatim.new("code\n", "=== heading three\n")]

    assert_equal expected, @RMP.parse(str).parts
  end

  def test_parse_verbatim_merge
    str = <<-STR
now is
   code

   code1
the time
    STR

    expected = [
      @RM::Paragraph.new('now is'),
      @RM::Verbatim.new("code\n", "\n", "code1\n"),
      @RM::Paragraph.new('the time'),
    ]

    assert_equal expected, @RMP.parse(str).parts
  end

  def test_parse_verbatim_merge2
    str = <<-STR
now is
   code

   code1

   code2
the time
    STR

    expected = [
      @RM::Paragraph.new('now is'),
      @RM::Verbatim.new("code\n", "\n", "code1\n", "\n", "code2\n"),
      @RM::Paragraph.new('the time'),
    ]

    assert_equal expected, @RMP.parse(str).parts
  end

  def test_parse_verbatim_multiline
    str = <<-STR
now is
   code
   code1
the time
    STR

    expected = [
      @RM::Paragraph.new('now is'),
      @RM::Verbatim.new("code\n", "code1\n"),
      @RM::Paragraph.new('the time'),
    ]

    assert_equal expected, @RMP.parse(str).parts
  end

  def test_parse_verbatim_multilevel
    str = <<-STR
now is the time
  code
 more code
for all good men
    STR

    expected = [
      @RM::Paragraph.new('now is the time'),
      @RM::Verbatim.new(" code\n",
                        "more code\n"),
      @RM::Paragraph.new('for all good men'),
    ]

    assert_equal expected, @RMP.parse(str).parts
  end

  def test_parse_verbatim_note
    str = <<-STR
  blah:: blah
    STR

    expected = [
      @RM::Verbatim.new("blah:: blah\n")]

    assert_equal expected, @RMP.parse(str).parts
  end

  def test_parse_verbatim_number
    str = <<-STR
  2. blah
    STR

    expected = [
      @RM::Verbatim.new("2. blah\n")]

    assert_equal expected, @RMP.parse(str).parts
  end

  def test_parse_verbatim_rule
    str = <<-STR
text

  --- lib/blah.rb.orig
  +++ lib/blah.rb
    STR

    expected = [
      @RM::Paragraph.new('text'),
      @RM::BlankLine.new,
      @RM::Verbatim.new("--- lib/blah.rb.orig\n",
                        "+++ lib/blah.rb\n")]

    assert_equal expected, @RMP.parse(str).parts
  end

  def test_parse_verbatim_rule2
    str = <<-STR.chomp
text

  ---
    STR

    expected = [
      @RM::Paragraph.new('text'),
      @RM::BlankLine.new,
      @RM::Verbatim.new("---")]

    assert_equal expected, @RMP.parse(str).parts
  end

  def test_parse_verbatim_trim
    str = <<-STR
now is
   code

   code1

the time
    STR

    expected = [
      @RM::Paragraph.new('now is'),
      @RM::Verbatim.new("code\n",
                        "\n",
                        "code1\n"),
      @RM::Paragraph.new('the time'),
    ]

    assert_equal expected, @RMP.parse(str).parts
  end

  def test_parse_verbatim_ualpha
    str = <<-STR
  B. blah
    STR

    expected = [
      @RM::Verbatim.new("B. blah\n")]

    assert_equal expected, @RMP.parse(str).parts
  end

  def test_parse_whitespace
    expected = [
      @RM::Paragraph.new('hello'),
    ]

    assert_equal expected, @RMP.parse('hello').parts

    expected = [
      @RM::Verbatim.new('hello '),
    ]

    assert_equal expected, @RMP.parse('  hello ').parts

    expected = [
      @RM::Verbatim.new('hello          '),
    ]

    assert_equal expected, @RMP.parse('                 hello          ').parts

    expected = [
      @RM::Paragraph.new('1'),
      @RM::Verbatim.new("2\n", ' 3'),
    ]

    assert_equal expected, @RMP.parse("1\n 2\n  3").parts

    expected = [
      @RM::Verbatim.new("1\n",
                        " 2\n",
                        "  3"),
    ]

    assert_equal expected, @RMP.parse("  1\n   2\n    3").parts

    expected = [
      @RM::Paragraph.new('1'),
      @RM::Verbatim.new("2\n",
                        " 3\n"),
      @RM::Paragraph.new('1'),
      @RM::Verbatim.new('2'),
    ]

    assert_equal expected, @RMP.parse("1\n 2\n  3\n1\n 2").parts

    expected = [
      @RM::Verbatim.new("1\n",
                        " 2\n",
                        "  3\n",
                        "1\n",
                        ' 2'),
    ]

    assert_equal expected, @RMP.parse("  1\n   2\n    3\n  1\n   2").parts

    expected = [
      @RM::Verbatim.new("1\n",
                        " 2\n",
                        "\n",
                        '  3'),
    ]

    assert_equal expected, @RMP.parse("  1\n   2\n\n    3").parts
  end

  def test_peek_token
    parser = util_parser

    assert_equal [:HEADER, 1, 0, 0], parser.peek_token

    assert_equal 8, parser.tokens.length
  end

  def test_skip
    parser = util_parser

    assert_equal [:HEADER, 1, 0, 0], parser.skip(:HEADER)

    assert_equal [:TEXT, 'Heading', 2, 0], parser.get

    assert_equal [:NEWLINE, "\n", 9, 0], parser.peek_token

    assert_raises RDoc::Markup::Parser::ParseError do
      parser.skip :NONE
    end

    assert_equal [:NEWLINE, "\n", 9, 0], parser.peek_token

    assert_equal nil, parser.skip(:NONE, false)

    assert_equal [:NEWLINE, "\n", 9, 0], parser.peek_token
  end

  def test_tokenize_bullet
    str = <<-STR
* l1
    STR

    expected = [
      [:BULLET,  '*',     0, 0],
      [:TEXT,    'l1',    2, 0],
      [:NEWLINE, "\n",    4, 0],
    ]

    assert_equal expected, @RMP.tokenize(str)
  end

  def test_tokenize_bullet_indent
    str = <<-STR
* l1
  * l1.1
    STR

    expected = [
      [:BULLET,  '*',     0, 0],
      [:TEXT,    'l1',    2, 0],
      [:NEWLINE, "\n",    4, 0],
      [:BULLET,  '*',     2, 1],
      [:TEXT,    'l1.1',  4, 1],
      [:NEWLINE, "\n",    8, 1],
    ]

    assert_equal expected, @RMP.tokenize(str)
  end

  def test_tokenize_heading
    str = <<-STR
= Heading
== Heading 2
    STR

    expected = [
      [:HEADER,  1,            0, 0],
      [:TEXT,    'Heading',    2, 0],
      [:NEWLINE, "\n",         9, 0],
      [:HEADER,  2,            0, 1],
      [:TEXT,    'Heading 2',  3, 1],
      [:NEWLINE, "\n",        12, 1],
    ]

    assert_equal expected, @RMP.tokenize(str)
  end

  def test_tokenize_heading_empty
    str = <<-STR
===
* bullet
    STR

    expected = [
      [:HEADER,  3,        0, 0],
      [:NEWLINE, "\n",     3, 0],
      [:BULLET,  "*",      0, 1],
      [:TEXT,    "bullet", 2, 1],
      [:NEWLINE, "\n",     8, 1],
    ]

    assert_equal expected, @RMP.tokenize(str)
  end

  def test_tokenize_heading_heading
    str = <<-STR
= =
    STR

    expected = [
      [:HEADER,  1,    0, 0],
      [:TEXT,    '=',  2, 0],
      [:NEWLINE, "\n", 3, 0],
    ]

    assert_equal expected, @RMP.tokenize(str)
  end

  def test_tokenize_heading_no_space
    str = <<-STR
=Heading
==Heading 2
    STR

    expected = [
      [:HEADER,  1,            0, 0],
      [:TEXT,    'Heading',    1, 0],
      [:NEWLINE, "\n",         8, 0],
      [:HEADER,  2,            0, 1],
      [:TEXT,    'Heading 2',  2, 1],
      [:NEWLINE, "\n",        11, 1],
    ]

    assert_equal expected, @RMP.tokenize(str)
  end

  def test_tokenize_label
    str = <<-STR
[cat] l1
[dog] l1.1
    STR

    expected = [
      [:LABEL,   'cat',   0, 0],
      [:TEXT,    'l1',    6, 0],
      [:NEWLINE, "\n",    8, 0],
      [:LABEL,   'dog',   0, 1],
      [:TEXT,    'l1.1',  6, 1],
      [:NEWLINE, "\n",   10, 1],
    ]

    assert_equal expected, @RMP.tokenize(str)
  end

  def test_tokenize_label_note
    str = <<-STR
[label]
  note::
    STR

    expected = [
      [:LABEL,   'label', 0, 0],
      [:NEWLINE, "\n",    7, 0],
      [:NOTE,    'note',  2, 1],
      [:NEWLINE, "\n",    8, 1],
    ]

    assert_equal expected, @RMP.tokenize(str)
  end

  def test_tokenize_lalpha
    str = <<-STR
a. l1
b. l1.1
    STR

    expected = [
      [:LALPHA,  'a',     0, 0],
      [:TEXT,    'l1',    3, 0],
      [:NEWLINE, "\n",    5, 0],
      [:LALPHA,  'b',     0, 1],
      [:TEXT,    'l1.1',  3, 1],
      [:NEWLINE, "\n",    7, 1],
    ]

    assert_equal expected, @RMP.tokenize(str)
  end

  def test_tokenize_note
    str = <<-STR
cat:: l1
dog:: l1.1
    STR

    expected = [
      [:NOTE,    'cat',   0, 0],
      [:TEXT,    'l1',    6, 0],
      [:NEWLINE, "\n",    8, 0],
      [:NOTE,    'dog',   0, 1],
      [:TEXT,    'l1.1',  6, 1],
      [:NEWLINE, "\n",   10, 1],
    ]

    assert_equal expected, @RMP.tokenize(str)
  end

  def test_tokenize_note_empty
    str = <<-STR
cat::
dog::
    STR

    expected = [
      [:NOTE,    'cat', 0, 0],
      [:NEWLINE, "\n",  5, 0],
      [:NOTE,    'dog', 0, 1],
      [:NEWLINE, "\n",  5, 1],
    ]

    assert_equal expected, @RMP.tokenize(str)
  end

  def test_tokenize_note_not
    str = <<-STR
Cat::Dog
    STR

    expected = [
      [:TEXT,    'Cat::Dog', 0, 0],
      [:NEWLINE, "\n",       8, 0],
    ]

    assert_equal expected, @RMP.tokenize(str)
  end

  def test_tokenize_number
    str = <<-STR
1. l1
2. l1.1
    STR

    expected = [
      [:NUMBER,  '1',     0, 0],
      [:TEXT,    'l1',    3, 0],
      [:NEWLINE, "\n",    5, 0],
      [:NUMBER,  '2',     0, 1],
      [:TEXT,    'l1.1',  3, 1],
      [:NEWLINE, "\n",    7, 1],
    ]

    assert_equal expected, @RMP.tokenize(str)
  end

  def test_tokenize_number_period
    str = <<-STR
1. blah blah blah
   l.
2. blah blah blah blah
   d.
    STR

    expected = [
      [:NUMBER,  "1",                    0, 0],
      [:TEXT,    "blah blah blah",       3, 0],
      [:NEWLINE, "\n",                  17, 0],

      [:TEXT,    "l.",                   3, 1],
      [:NEWLINE, "\n",                   5, 1],

      [:NUMBER,  "2",                    0, 2],
      [:TEXT,    "blah blah blah blah",  3, 2],
      [:NEWLINE, "\n",                  22, 2],

      [:TEXT,    "d.",                   3, 3],
      [:NEWLINE, "\n",                   5, 3]
    ]

    assert_equal expected, @RMP.tokenize(str)
  end

  def test_tokenize_number_period_continue
    str = <<-STR
1. blah blah blah
   l.  more stuff
2. blah blah blah blah
   d. other stuff
    STR

    expected = [
      [:NUMBER,  "1",                    0, 0],
      [:TEXT,    "blah blah blah",       3, 0],
      [:NEWLINE, "\n",                  17, 0],

      [:LALPHA,  "l",                    3, 1],
      [:TEXT,    "more stuff",           7, 1],
      [:NEWLINE, "\n",                  17, 1],

      [:NUMBER,  "2",                    0, 2],
      [:TEXT,    "blah blah blah blah",  3, 2],
      [:NEWLINE, "\n",                  22, 2],

      [:LALPHA,  "d",                    3, 3],
      [:TEXT,    "other stuff",          6, 3],
      [:NEWLINE, "\n",                  17, 3]
    ]

    assert_equal expected, @RMP.tokenize(str)
  end

  def test_tokenize_paragraphs
    str = <<-STR
now is
the time

for all
    STR

    expected = [
      [:TEXT,    'now is',   0, 0],
      [:NEWLINE, "\n",       6, 0],
      [:TEXT,    'the time', 0, 1],
      [:NEWLINE, "\n",       8, 1],
      [:NEWLINE, "\n",       0, 2],
      [:TEXT,    'for all',  0, 3],
      [:NEWLINE, "\n",       7, 3],
    ]

    assert_equal expected, @RMP.tokenize(str)
  end

  def test_tokenize_rule
    str = <<-STR
---

--- blah ---
    STR

    expected = [
      [:RULE,    1,            0, 0],
      [:NEWLINE, "\n",         3, 0],
      [:NEWLINE, "\n",         0, 1],
      [:TEXT, "--- blah ---",  0, 2],
      [:NEWLINE, "\n",        12, 2],
    ]

    assert_equal expected, @RMP.tokenize(str)
  end

  def test_tokenize_ualpha
    str = <<-STR
A. l1
B. l1.1
    STR

    expected = [
      [:UALPHA,  'A',     0, 0],
      [:TEXT,    'l1',    3, 0],
      [:NEWLINE, "\n",    5, 0],
      [:UALPHA,  'B',     0, 1],
      [:TEXT,    'l1.1',  3, 1],
      [:NEWLINE, "\n",    7, 1],
    ]

    assert_equal expected, @RMP.tokenize(str)
  end

  def test_tokenize_verbatim_heading
    str = <<-STR
Example heading:

   === heading three
    STR

    expected = [
      [:TEXT,    'Example heading:',  0, 0],
      [:NEWLINE, "\n",               16, 0],
      [:NEWLINE, "\n",                0, 1],
      [:HEADER,  3,                   3, 2],
      [:TEXT,    'heading three',     7, 2],
      [:NEWLINE, "\n",               20, 2],
    ]

    assert_equal expected, @RMP.tokenize(str)
  end

  def test_tokenize_verbatim_rule
    str = <<-STR
  Verbatim section here that is double-underlined
  ===============================================
    STR

    expected = [
      [:TEXT,    'Verbatim section here that is double-underlined',  2, 0],
      [:NEWLINE, "\n",                                              49, 0],
      [:HEADER,  47,                                                 2, 1],
      [:NEWLINE, "\n",                                              49, 1],
    ]

    assert_equal expected, @RMP.tokenize(str)
  end

  def test_tokenize_verbatim_rule_fancy
    str = <<-STR
  A
    b
  ===============================================
    c
    STR

    expected = [
      [:TEXT,    'A',   2, 0],
      [:NEWLINE, "\n",  3, 0],
      [:TEXT,    'b',   4, 1],
      [:NEWLINE, "\n",  5, 1],
      [:HEADER,  47,    2, 2],
      [:NEWLINE, "\n", 49, 2],
      [:TEXT,    'c',   4, 3],
      [:NEWLINE, "\n",  5, 3],
    ]

    assert_equal expected, @RMP.tokenize(str)
  end

  # HACK move to Verbatim test case
  def test_verbatim_normalize
    v = @RM::Verbatim.new "foo\n", "\n", "\n", "bar\n"

    v.normalize

    assert_equal ["foo\n", "\n", "bar\n"], v.parts

    v = @RM::Verbatim.new "foo\n", "\n"

    v.normalize

    assert_equal ["foo\n"], v.parts
  end

  def test_unget
    parser = util_parser

    parser.get

    parser.unget

    assert_equal [:HEADER, 1, 0, 0], parser.peek_token

    assert_raises @RMP::Error do
      parser.unget
    end

    assert_equal 8, parser.tokens.length
  end

  def util_parser
    str = <<-STR
= Heading

Some text here
some more text over here
    STR

    @parser = @RMP.new
    @parser.tokenize str
    @parser
  end

end

