require File.expand_path '../xref_test_case', __FILE__

class TestRDocNormalClass < XrefTestCase

  def test_ancestors_class
    top_level = RDoc::TopLevel.new 'file.rb'
    klass = top_level.add_class RDoc::NormalClass, 'Klass'
    incl = RDoc::Include.new 'Incl', ''

    sub_klass = klass.add_class RDoc::NormalClass, 'SubClass', 'Klass'
    sub_klass.add_include incl

    assert_equal [incl.name, klass], sub_klass.ancestors
  end

  def test_definition
    c = RDoc::NormalClass.new 'C'

    assert_equal 'class C', c.definition
  end

end

