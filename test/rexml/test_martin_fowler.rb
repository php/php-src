require 'test/unit'
require 'rexml/document'

class OrderTester < Test::Unit::TestCase
  DOC = <<END
<paper>
<title>Remove this element and figs order differently</title>
<figure src="fig1"/>
<figure src="fig2"/>
<p>Para of text</p>
<p>Remove this and figs order differently</p>
<section>
<figure src="fig3"/>
</section>
<figure src="fig4"/>
</paper>
END

  def initialize n
    @doc = REXML::Document.new(DOC)
    @figs = REXML::XPath.match(@doc,'//figure')
    @names = @figs.collect {|f| f.attributes['src']}
    super
  end
  def test_fig1
    assert_equal 'fig1', @figs[0].attributes['src']
  end
  def test_fig2
    assert_equal 'fig2', @figs[1].attributes['src']
  end
  def test_fig3
    assert_equal 'fig3', @figs[2].attributes['src']
  end
  def test_fig4
    assert_equal 'fig4', @figs[3].attributes['src']
  end
end
