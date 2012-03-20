require "test/unit/testcase"
require "rexml/document"
require "rexml/xpath"
require "rexml/parsers/xpathparser"

class XPathPredicateTester < Test::Unit::TestCase
  include REXML
  SRC=<<-EOL
  <article>
     <section role="subdivision" id="1">
        <para>free flowing text.</para>
     </section>
     <section role="division">
        <section role="subdivision" id="2">
           <para>free flowing text.</para>
        </section>
        <section role="division">
           <para>free flowing text.</para>
        </section>
     </section>
  </article>
  EOL

  def setup
    @doc = REXML::Document.new( SRC )
    @parser = REXML::Parsers::XPathParser.new

  end

  def test_predicates_parent
    path = '//section[../self::section[@role="division"]]'
    m = do_path( path )
    assert_equal( 2, m.size )
    assert_equal( "2", m[0].attributes["id"] )
    assert_nil( m[1].attributes["id"] )
  end

  def test_predicates_single
    path = '//section[@role="subdivision" and not(../self::section[@role="division"])]'
    m = do_path( path )
    assert_equal( 1, m.size )
    assert_equal( "1", m[0].attributes["id"] )
  end

  def test_predicates_multi
    path = '//section[@role="subdivision"][not(../self::section[@role="division"])]'
    m = do_path( path )
    assert_equal( 1, m.size )
    assert_equal( "1", m[0].attributes["id"] )
  end

  def do_path( path )
    m = REXML::XPath.match( @doc, path )
    #puts path, @parser.parse( path ).inspect
    return m
  end

  def test_get_no_siblings_terminal_nodes
    source = <<-XML
<a>
  <b number='1' str='abc'>TEXT1</b>
  <c number='1'/>
  <c number='2' str='def'>
    <b number='3'/>
    <d number='1' str='abc'>TEXT2</d>
    <b number='2'><!--COMMENT--></b>
  </c>
</a>
XML
    doc = REXML::Document.new(source)
    predicate = "count(child::node()|" +
                      "following-sibling::node()|" +
                      "preceding-sibling::node())=0"
    m = REXML::XPath.match(doc, "/descendant-or-self::node()[#{predicate}]")
    assert_equal( [REXML::Text.new("TEXT1"),
                   REXML::Text.new("TEXT2"),
                   REXML::Comment.new("COMMENT")],
                  m )
  end
end
