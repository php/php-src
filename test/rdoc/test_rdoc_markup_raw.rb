require 'pp'
require 'rubygems'
require 'minitest/autorun'
require 'rdoc/markup'

class TestRDocMarkupRaw < MiniTest::Unit::TestCase

  def setup
    @RM = RDoc::Markup
    @p = @RM::Raw.new
  end

  def mu_pp obj
    s = ''
    s = PP.pp obj, s
    s.force_encoding Encoding.default_external if defined? Encoding
    s.chomp
  end

  def test_push
    @p.push 'hi', 'there'

    assert_equal @RM::Raw.new('hi', 'there'), @p
  end

end

