require 'rubygems'
require 'minitest/autorun'
require 'rdoc'
require 'rdoc/markup'
require 'rdoc/markup/inline'
require 'rdoc/markup/to_html_crossref'

class TestRDocMarkupAttributeManager < MiniTest::Unit::TestCase

  def setup
    @am = RDoc::Markup::AttributeManager.new

    @bold_on  = @am.changed_attribute_by_name([], [:BOLD])
    @bold_off = @am.changed_attribute_by_name([:BOLD], [])

    @tt_on    = @am.changed_attribute_by_name([], [:TT])
    @tt_off   = @am.changed_attribute_by_name([:TT], [])

    @em_on    = @am.changed_attribute_by_name([], [:EM])
    @em_off   = @am.changed_attribute_by_name([:EM], [])

    @bold_em_on   = @am.changed_attribute_by_name([], [:BOLD] | [:EM])
    @bold_em_off  = @am.changed_attribute_by_name([:BOLD] | [:EM], [])

    @em_then_bold = @am.changed_attribute_by_name([:EM], [:EM] | [:BOLD])

    @em_to_bold   = @am.changed_attribute_by_name([:EM], [:BOLD])

    @am.add_word_pair("{", "}", :WOMBAT)
    @wombat_on    = @am.changed_attribute_by_name([], [:WOMBAT])
    @wombat_off   = @am.changed_attribute_by_name([:WOMBAT], [])
  end

  def crossref(text)
    crossref_bitmap = RDoc::Markup::Attribute.bitmap_for(:_SPECIAL_) |
                      RDoc::Markup::Attribute.bitmap_for(:CROSSREF)

    [ @am.changed_attribute_by_name([], [:CROSSREF, :_SPECIAL_]),
      RDoc::Markup::Special.new(crossref_bitmap, text),
      @am.changed_attribute_by_name([:CROSSREF, :_SPECIAL_], [])
    ]
  end

  def test_adding
    assert_equal(["cat ", @wombat_on, "and", @wombat_off, " dog" ],
                  @am.flow("cat {and} dog"))
    #assert_equal(["cat {and} dog" ], @am.flow("cat \\{and} dog"))
  end

  def test_add_word_pair
    @am.add_word_pair '%', '&', 'percent and'

    assert @am.word_pair_map.include?(/(%)(\S+)(&)/)
    assert @am.protectable.include?('%')
    assert !@am.protectable.include?('&')
  end

  def test_add_word_pair_angle
    e = assert_raises ArgumentError do
      @am.add_word_pair '<', '>', 'angles'
    end

    assert_equal "Word flags may not start with '<'", e.message
  end

  def test_add_word_pair_matching
    @am.add_word_pair '^', '^', 'caret'

    assert @am.matching_word_pairs.include?('^')
    assert @am.protectable.include?('^')
  end

  def test_basic
    assert_equal(["cat"], @am.flow("cat"))

    assert_equal(["cat ", @bold_on, "and", @bold_off, " dog"],
                  @am.flow("cat *and* dog"))

    assert_equal(["cat ", @bold_on, "AND", @bold_off, " dog"],
                  @am.flow("cat *AND* dog"))

    assert_equal(["cat ", @em_on, "And", @em_off, " dog"],
                  @am.flow("cat _And_ dog"))

    assert_equal(["cat *and dog*"], @am.flow("cat *and dog*"))

    assert_equal(["*cat and* dog"], @am.flow("*cat and* dog"))

    assert_equal(["cat *and ", @bold_on, "dog", @bold_off],
                  @am.flow("cat *and *dog*"))

    assert_equal(["cat ", @em_on, "and", @em_off, " dog"],
                  @am.flow("cat _and_ dog"))

    assert_equal(["cat_and_dog"],
                  @am.flow("cat_and_dog"))

    assert_equal(["cat ", @tt_on, "and", @tt_off, " dog"],
                  @am.flow("cat +and+ dog"))

    assert_equal(["cat ", @bold_on, "a_b_c", @bold_off, " dog"],
                  @am.flow("cat *a_b_c* dog"))

    assert_equal(["cat __ dog"],
                  @am.flow("cat __ dog"))

    assert_equal(["cat ", @em_on, "_", @em_off, " dog"],
                  @am.flow("cat ___ dog"))
  end

  def test_bold
    assert_equal [@bold_on, 'bold', @bold_off],
                 @am.flow("*bold*")

    assert_equal [@bold_on, 'Bold:', @bold_off],
                 @am.flow("*Bold:*")

    assert_equal [@bold_on, '\\bold', @bold_off],
                 @am.flow("*\\bold*")
  end

  def test_bold_html_escaped
    assert_equal ['cat <b>dog</b>'], @am.flow('cat \<b>dog</b>')
  end

  def test_combined
    assert_equal(["cat ", @em_on, "and", @em_off, " ", @bold_on, "dog", @bold_off],
                  @am.flow("cat _and_ *dog*"))

    assert_equal(["cat ", @em_on, "a__nd", @em_off, " ", @bold_on, "dog", @bold_off],
                  @am.flow("cat _a__nd_ *dog*"))
  end

  def test_convert_attrs
    str = '+foo+'
    attrs = RDoc::Markup::AttrSpan.new str.length

    @am.convert_attrs str, attrs

    assert_equal "\000foo\000", str

    str = '+:foo:+'
    attrs = RDoc::Markup::AttrSpan.new str.length

    @am.convert_attrs str, attrs

    assert_equal "\000:foo:\000", str

    str = '+x-y+'
    attrs = RDoc::Markup::AttrSpan.new str.length

    @am.convert_attrs str, attrs

    assert_equal "\000x-y\000", str
  end

  def test_html_like_em_bold
    assert_equal ["cat ", @em_on, "and ", @em_to_bold, "dog", @bold_off],
                  @am.flow("cat <i>and </i><b>dog</b>")
  end

  def test_html_like_em_bold_SGML
    assert_equal ["cat ", @em_on, "and ", @em_to_bold, "dog", @bold_off],
                  @am.flow("cat <i>and <b></i>dog</b>")
  end

  def test_html_like_em_bold_nested_1
    assert_equal(["cat ", @bold_em_on, "and", @bold_em_off, " dog"],
                  @am.flow("cat <i><b>and</b></i> dog"))
  end

  def test_html_like_em_bold_nested_2
    assert_equal ["cat ", @em_on, "and ", @em_then_bold, "dog", @bold_em_off],
                  @am.flow("cat <i>and <b>dog</b></i>")
  end

  def test_html_like_em_bold_nested_mixed_case
    assert_equal ["cat ", @em_on, "and ", @em_then_bold, "dog", @bold_em_off],
                  @am.flow("cat <i>and <B>dog</B></I>")
  end

  def test_html_like_em_bold_mixed_case
    assert_equal ["cat ", @em_on, "and", @em_off, " ", @bold_on, "dog", @bold_off],
                  @am.flow("cat <i>and</i> <B>dog</b>")
  end

  def test_html_like_teletype
    assert_equal ["cat ", @tt_on, "dog", @tt_off],
                 @am.flow("cat <tt>dog</Tt>")
  end

  def test_html_like_teletype_em_bold_SGML
    assert_equal [@tt_on, "cat", @tt_off, " ", @em_on, "and ", @em_to_bold, "dog", @bold_off],
                  @am.flow("<tt>cat</tt> <i>and <b></i>dog</b>")
  end

  def test_protect
    assert_equal(['cat \\ dog'],
                 @am.flow('cat \\ dog'))

    assert_equal(["cat <tt>dog</Tt>"],
                 @am.flow("cat \\<tt>dog</Tt>"))

    assert_equal(["cat ", @em_on, "and", @em_off, " <B>dog</b>"],
                  @am.flow("cat <i>and</i> \\<B>dog</b>"))

    assert_equal(["*word* or <b>text</b>"],
                 @am.flow("\\*word* or \\<b>text</b>"))

    assert_equal(["_cat_", @em_on, "dog", @em_off],
                  @am.flow("\\_cat_<i>dog</i>"))
  end

  def test_special
    @am.add_special(RDoc::Markup::ToHtmlCrossref::CROSSREF_REGEXP, :CROSSREF)

    #
    # The apostrophes in "cats'" and "dogs'" suppress the flagging of these
    # words as potential cross-references, which is necessary for the unit
    # tests.  Unfortunately, the markup engine right now does not actually
    # check whether a cross-reference is valid before flagging it.
    #
    assert_equal(["cats'"], @am.flow("cats'"))

    assert_equal(["cats' ", crossref("#fred"), " dogs'"].flatten,
                  @am.flow("cats' #fred dogs'"))

    assert_equal([crossref("#fred"), " dogs'"].flatten,
                  @am.flow("#fred dogs'"))

    assert_equal(["cats' ", crossref("#fred")].flatten, @am.flow("cats' #fred"))
  end

  def test_tt_html
    assert_equal [@tt_on, '"\n"', @tt_off],
                 @am.flow('<tt>"\n"</tt>')
  end

end

