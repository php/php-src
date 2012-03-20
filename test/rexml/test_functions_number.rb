require 'rexml/document'
require 'test/unit'
require 'rexml/functions'

class TC_Rexml_Functions_Number < Test::Unit::TestCase

  def test_functions_number_int
    telem = REXML::Element.new("elem")
    telem.text="9"
    assert_equal(9, REXML::Functions::number(telem))
  end
  def test_functions_number_float
    telem = REXML::Element.new("elem")
    telem.text="10.4"
    assert_equal(10.4, REXML::Functions::number(telem))
  end
  def test_functions_number_negative_int
    telem = REXML::Element.new("elem")
    telem.text="-9"
    assert_equal(-9, REXML::Functions::number(telem))
  end
  def test_functions_number_negative_float
    telem = REXML::Element.new("elem")
    telem.text="-9.13"
    assert_equal(-9.13, REXML::Functions::number(telem))
  end
  #def test_functions_number_scientific_notation
  #  telem = REXML::Element.new("elem")
  #  telem.text="9.13E12"
  #  assert_equal(9.13E12, REXML::Functions::number(telem))
  #end
end
