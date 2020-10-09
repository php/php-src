--TEST--
xml_parse_into_struct/umlauts in tags
--SKIPIF--
<?php
include("skipif.inc");
if(strtoupper("����") != "����")
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

$xmldata = '<?xml version="1.0" encoding="ISO-8859-1"?><��� ���="���">���</���>';
$parser = xml_parser_create('ISO-8859-1');
xml_set_element_handler($parser, "startHandler", "endHandler");
xml_parse_into_struct($parser, $xmldata, $struct, $index);
var_dump($struct);
?>
--EXPECT--
string(3) "���"
array(1) {
  ["���"]=>
  string(3) "���"
}
string(3) "���"
array(1) {
  [0]=>
  array(5) {
    ["tag"]=>
    string(3) "���"
    ["type"]=>
    string(8) "complete"
    ["level"]=>
    int(1)
    ["attributes"]=>
    array(1) {
      ["���"]=>
      string(3) "���"
    }
    ["value"]=>
    string(3) "���"
  }
}
