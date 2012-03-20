require 'rubygems'
require 'cgi'
require 'minitest/autorun'
require 'rdoc'
require 'rdoc/code_objects'

class TestRDocContextSection < MiniTest::Unit::TestCase

  def setup
    @S = RDoc::Context::Section
    @s = @S.new nil, 'section', '# comment'
  end

  def test_aref
    assert_equal 'section', @s.aref

    assert_equal '5Buntitled-5D', @S.new(nil, nil, nil).aref

    assert_equal 'one+two', @S.new(nil, 'one two', nil).aref
  end

  def test_comment_equals
    @s.comment = "# :section: section\n"

    assert_equal "# comment", @s.comment

    @s.comment = "# :section: section\n# other"

    assert_equal "# comment\n# ---\n# other", @s.comment

    s = @S.new nil, nil, nil

    s.comment = "# :section:\n# other"

    assert_equal "# other", s.comment
  end

  def test_extract_comment
    assert_equal '',    @s.extract_comment('')
    assert_equal '',    @s.extract_comment("# :section: b\n")
    assert_equal '# c', @s.extract_comment("# :section: b\n# c")
    assert_equal '# c', @s.extract_comment("# a\n# :section: b\n# c")
  end

  def test_sequence
    _, err = capture_io do
      assert_match(/\ASEC\d{5}\Z/, @s.sequence)
    end

    assert_equal "#{@S}#sequence is deprecated, use #aref\n", err
  end

end

