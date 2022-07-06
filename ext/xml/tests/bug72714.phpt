--TEST--
Bug #72714 (_xml_startElementHandler() segmentation fault)
--SKIPIF--
<?php
if (!extension_loaded('xml')) die('skip xml extension not available');
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
Warning: xml_parser_set_option(): tagstart ignored, because it is out of range in %s on line %d
string(9) "NS1:TOTAL"
string(0) ""
