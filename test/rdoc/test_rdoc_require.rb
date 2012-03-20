require File.expand_path '../xref_test_case', __FILE__

class TestRDocRequire < XrefTestCase

  def setup
    super

    @req = RDoc::Require.new 'foo', 'comment'
  end

  def test_initialize
    assert_equal 'foo', @req.name

    req = RDoc::Require.new '"foo"', ''
    assert_equal 'foo', @req.name

    req = RDoc::Require.new '\'foo\'', ''
    assert_equal 'foo', @req.name

    req = RDoc::Require.new '|foo|', ''
    assert_equal 'foo', @req.name, 'for fortran?'
  end

end

