--TEST--
Bug #72714 (xml_startElementHandler() segmentation fault)
--EXTENSIONS--
xml
--SKIPIF--
<?php
if (PHP_INT_SIZE < 8) die('skip this test is for 64bit builds only');
?>
--FILE--
<?php
function startElement($parser, $name, $attribs) {
    var_dump($name);
}

function endElement($parser, $name) {}

function parse($tagstart) {
    $xml = '<ns1:total>867</ns1:total>';

    $xml_parser = xml_parser_create();
    xml_set_element_handler($xml_parser, 'startElement', 'endElement');

    xml_parser_set_option($xml_parser, XML_OPTION_SKIP_TAGSTART, $tagstart);
    xml_parse($xml_parser, $xml);

    xml_parser_free($xml_parser);
}

parse(3015809298423721);
parse(20);
?>
--EXPECTF--
Warning: xml_parser_set_option(): Argument #3 ($value) must be between 0 and 2147483647 for option XML_OPTION_SKIP_TAGSTART in %s on line %d
string(9) "NS1:TOTAL"
string(0) ""
