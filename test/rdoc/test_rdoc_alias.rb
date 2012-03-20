require File.expand_path '../xref_test_case', __FILE__

class TestRDocAlias < XrefTestCase

  def test_to_s
    a = RDoc::Alias.new nil, 'a', 'b', ''
    a.parent = @c2

    assert_equal 'alias: b -> #a in: RDoc::NormalClass C2 < Object', a.to_s
  end

end

