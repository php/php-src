--TEST--
xml_parse_into_struct/umlauts in tags
--SKIPIF--
<?php // vim600: syn=php
include("skipif.inc");
if(strtoupper("äöüß") != "ÄÖÜß")
{
	die("skip strtoupper on non-ascii not supported on this platform");
}
?>
--FILE--
<?php
function startHandler($parser,$tag,$attr)
{
	var_dump($tag,$attr);
}

function endHandler($parser,$tag)
{ 
	var_dump($tag); 
}

$xmldata = '<?xml version="1.0" encoding="ISO-8859-1"?><äöü üäß="Üäß">ÄÖÜ</äöü>';
$parser = xml_parser_create('ISO-8859-1');
xml_set_element_handler($parser, "startHandler", "endHandler");
xml_parse_into_struct($parser, $xmldata, $struct, $index);
var_dump($struct);
?>
--EXPECT--
string(3) "ÄÖÜ"
array(1) {
  ["ÜÄß"]=>
  string(3) "Üäß"
}
string(3) "ÄÖÜ"
array(1) {
  [0]=>
  array(5) {
    ["tag"]=>
    string(3) "ÄÖÜ"
    ["type"]=>
    string(8) "complete"
    ["level"]=>
    int(1)
    ["attributes"]=>
    array(1) {
      ["ÜÄß"]=>
      string(3) "Üäß"
    }
    ["value"]=>
    string(3) "ÄÖÜ"
  }
}
