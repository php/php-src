--TEST--
GH-20439 (xml_set_default_handler() does not properly handle special characters in attributes when passing data to callback)
--EXTENSIONS--
xml
--FILE--
<?php

$x = xml_parser_create_ns('utf-8');
xml_set_default_handler($x, function( $_parser, $data ) { var_dump($data); });

$input = "<!-- xxx --><foo   attr1='\"&lt;&quot;&#9;&#x0A;&#x0D;&#13;𐍅' attr2=\"&quot;&lt;\"></foo>";
$inputs = str_split($input);

// Test chunked parser wrt non-progressive parser
foreach ($inputs as $input) {
	xml_parse($x, $input, false);
}
xml_parse($x, "", true);

?>
--EXPECT--
string(12) "<!-- xxx -->"
string(71) "<foo   attr1='"&lt;&quot;&#9;&#x0A;&#x0D;&#13;𐍅' attr2="&quot;&lt;">"
string(6) "</foo>"
