require File.expand_path '../xref_test_case', __FILE__

class TestRDocSingleClass < XrefTestCase

  def test_definition
    c = RDoc::SingleClass.new 'C'

    assert_equal 'class << C', c.definition
  end

end

