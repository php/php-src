require "test/unit/testcase"

require 'rexml/document'
require 'rexml/entity'
require 'rexml/source'

class EntityTester < Test::Unit::TestCase
  def test_parse_general_decl
    simple = "<!ENTITY foo 'bar'>"
    simple =~ /#{REXML::Entity::GEDECL}/
    assert $&
    assert_equal simple, $&

    REXML::Entity::ENTITYDECL =~ simple
    assert REXML::Entity::matches?(simple)
    match = REXML::Entity::ENTITYDECL.match(simple)
    assert_equal 'foo', match[1]
    assert_equal "'bar'", match[2]

    simple = '<!ENTITY Pub-Status
    "This is a pre-release of the specification.">'
    assert REXML::Entity::matches?(simple)
    match = REXML::Entity::ENTITYDECL.match(simple)
    assert_equal 'Pub-Status', match[1]
    assert_equal '"This is a pre-release of the specification."', match[2]

    txt = '"This is a
    pre-release of <the> specification."'
    simple = "<!ENTITY     Pub-Status
    #{txt}>"
    assert REXML::Entity::matches?(simple)
    match = REXML::Entity::ENTITYDECL.match(simple)
    assert_equal 'Pub-Status', match[1]
    assert_equal txt, match[2]
  end

  def test_parse_external_decl
    zero = '<!ENTITY open-hatch SYSTEM "http://www.textuality.com/boilerplate/OpenHatch.xml" >'
    one = '<!ENTITY open-hatch
              SYSTEM "http://www.textuality.com/boilerplate/OpenHatch.xml">'
    two = '<!ENTITY open-hatch
              PUBLIC "-//Textuality//TEXT Standard open-hatch boilerplate//EN"
              "http://www.textuality.com/boilerplate/OpenHatch.xml">'
    three = '<!ENTITY hatch-pic
              SYSTEM "../grafix/OpenHatch.gif"
              NDATA gif >'
    assert REXML::Entity::matches?(zero)
    assert REXML::Entity::matches?(one)
    assert REXML::Entity::matches?(two)
    assert REXML::Entity::matches?(three)
  end

  def test_parse_entity
    one = %q{<!ENTITY % YN '"Yes"'>}
    two = %q{<!ENTITY WhatHeSaid "He said %YN;">}
    assert REXML::Entity::matches?(one)
    assert REXML::Entity::matches?(two)
  end

  def test_constructor
    one = [ %q{<!ENTITY % YN '"Yes"'>},
      %q{<!ENTITY % YN2 "Yes">},
      %q{<!ENTITY WhatHeSaid "He said %YN;">},
      '<!ENTITY open-hatch
              SYSTEM "http://www.textuality.com/boilerplate/OpenHatch.xml">',
      '<!ENTITY open-hatch2
              PUBLIC "-//Textuality//TEXT Standard open-hatch boilerplate//EN"
              "http://www.textuality.com/boilerplate/OpenHatch.xml">',
      '<!ENTITY hatch-pic
              SYSTEM "../grafix/OpenHatch.gif"
              NDATA gif>' ]
    source = %q{<!DOCTYPE foo [
      <!ENTITY % YN '"Yes"'>
      <!ENTITY % YN2 "Yes">
      <!ENTITY WhatHeSaid "He said %YN;">
      <!ENTITY open-hatch
              SYSTEM "http://www.textuality.com/boilerplate/OpenHatch.xml">
      <!ENTITY open-hatch2
              PUBLIC "-//Textuality//TEXT Standard open-hatch boilerplate//EN"
              "http://www.textuality.com/boilerplate/OpenHatch.xml">
      <!ENTITY hatch-pic
              SYSTEM "../grafix/OpenHatch.gif"
              NDATA gif>
    ]>}

    d = REXML::Document.new( source )
    dt = d.doctype
    c = 0
    dt.each do |child|
      if child.kind_of? REXML::Entity
        str = one[c].tr("\r\n\t", '   ').squeeze(" ")
        assert_equal str, child.to_s
        c+=1
      end
    end
  end

  def test_replace_entities
    source = "<!DOCTYPE blah [\n<!ENTITY foo \"bar\">\n]><a>&foo;</a>"
    doc = REXML::Document.new(source)
    assert_equal 'bar', doc.root.text
    out = ''
    doc.write out
    assert_equal source, out
  end

  def test_raw
    source = '<!DOCTYPE foo [
<!ENTITY ent "replace">
]><a>replace &ent;</a>'
    doc = REXML::Document.new( source, {:raw=>:all})
    assert_equal('replace &ent;', doc.root.get_text.to_s)
    assert_equal(source, doc.to_s)
  end

  def test_lazy_evaluation
    source = '<!DOCTYPE foo [
<!ENTITY ent "replace">
]><a>replace &ent;</a>'
    doc = REXML::Document.new( source )
    assert_equal(source, doc.to_s)
    assert_equal("replace replace", doc.root.text)
    assert_equal(source, doc.to_s)
  end

  # Contributed (not only test, but bug fix!!) by Kouhei Sutou
  def test_entity_replacement
    source = %q{<!DOCTYPE foo [
    <!ENTITY % YN '"Yes"'>
    <!ENTITY WhatHeSaid "He said %YN;">]>
    <a>&WhatHeSaid;</a>}

    d = REXML::Document.new( source )
    dt = d.doctype
    assert_equal( '"Yes"', dt.entities[ "YN" ].value )
    assert_equal( 'He said "Yes"', dt.entities[ "WhatHeSaid" ].value )
    assert_equal( 'He said "Yes"', d.elements[1].text )
  end

  # More unit tests from Kouhei.  I looove users who give me unit tests.
  def test_entity_insertions
    assert_equal("&amp;", REXML::Text.new("&amp;", false, nil, true).to_s)
    #assert_equal("&", REXML::Text.new("&amp;", false, false).to_s)
  end

  def test_single_pass_unnormalization # ticket 123
    assert_equal '&amp;&', REXML::Text::unnormalize('&#38;amp;&amp;')
  end
end
