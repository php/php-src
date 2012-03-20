require 'rexml_test_utils'
require 'rexml/document'
require 'zlib'

class OrderTester < Test::Unit::TestCase
  include REXMLTestUtils

  TESTDOC = <<END
<a>
  <b/>
  <x id='1'/>
  <c/>
  <d>
    <x id='2'/>
  </d>
  <x id='3'/>
</a>
END

   def setup
     @doc = REXML::Document.new(TESTDOC)
     @items = REXML::XPath.match(@doc,'//x')
   end
   def test_first_element
     assert_equal '1', @items[0].attributes['id']
   end
   def test_second_element
     assert_equal '2', @items[1].attributes['id']
   end
   def test_third_element
     assert_equal '3', @items[2].attributes['id']
   end
   def test_order
     d = REXML::Document.new( "<a><x id='1'/><x id='2'/><x id='3'/>
     <x id='4'/><x id='5'/></a>" )
     items = REXML::XPath.match( d, '//x' )
     assert_equal( %w{1 2 3 4 5}, items.collect{|e| e.attributes['id']} )
     d = REXML::Document.new( "<a>
      <x><z><y id='1'/><y id='2'/></z><y id='3'/></x>
      <x><y id='4'/></x></a>" )
     items = REXML::XPath.match( d, '//y' )
     assert_equal( %w{1 2 3 4}, items.collect{|e| e.attributes['id']} )
   end
   # Provided by Tom Talbott
   def test_more_ordering
     doc = REXML::Document.new(Zlib::GzipReader.open(fixture_path('LostineRiver.kml.gz'), encoding: 'utf-8'))
     actual = [
        "Head south from Phinney Ave N",
        "Turn left at N 36th St",
        "Turn right at Fremont Ave N",
        "Continue on 4th Ave N",
        "Turn left at Westlake Ave N",
        "Bear right at 9th Ave N",
        "Turn left at Mercer St",
        "Take the I-5 ramp",
        "Take the I-5 S ramp",
        "Take the I-90 E exit #164 to Bellevue/Spokane/4th Ave S.",
        "Take the I-90 E ramp to Bellevue/Spokane",
        "Take exit #137 to Wanapum Dam/Richland",
        "Bear right at WA-26",
        "Bear right and head toward WA-243",
        "Continue on WA-243",
        "Bear right at WA-24",
        "Continue on WA-240",
        "Turn right at WA-240 E",
        "Take the I-182 W ramp to Yakima (I-82)/Pendleton",
        "Take the I-82 E ramp to Umatilla/Pendleton",
        "Take the I-84 E ramp to Pendleton",
        "Take the OR-82 exit #261 to La Grande/Elgin",
        "Turn right at Island Ave",
        "Continue on W 1st St",
        "Turn left at N McAlister Rd",
        "Bear right at OR-82",
        "Continue on Wallowa Lake Hwy",
        "Continue on OR-82",
        "Continue on Ruckman Ave",
        "Continue on OR-82",
        "Continue on S 8th Ave",
        "Turn right at Albany St",
        "Continue on OR-82",
        "Continue on Wallowa Lake Hwy",
        "Continue on N Madison St",
        "Bear left at W 1st St",
        "Continue on Wallowa Lake Hwy",
        "Continue on Water St",
        "Bear right at Lostine River Rd",
        "Bear right and head toward Lostine River Rd",
        "Turn right at Lostine River Rd",
        "Continue on NF-8210",
        "Turn right and head toward NF-8210",
        "Turn right at NF-8210",
        "",
        "Route"
     ]
     count = 0
     REXML::XPath.each( doc, "//Placemark") { |element|
       n = element.elements["name"].text.squeeze(" ")
       assert_equal( actual[count], n ) unless n =~ /Arrive at/
       count += 1
     }
   end
end
