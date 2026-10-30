--TEST--
GH-20439 (xml_set_default_handler() does not properly handle special characters in attributes when passing data to callback) - closing solidus variant
--EXTENSIONS--
xml
--SKIPIF--
<?php
require __DIR__ . '/libxml_expat_skipif.inc';
skipif(want_expat: false);
?>
--FILE--
<?php

$x = xml_parser_create_ns('utf-8');
xml_set_default_handler($x, function( $_parser, $data ) { var_dump($data); });

$input = "<ns:test xmlns:ns='urn:x'   />";
xml_parse($x, $input, true);

?>
--EXPECT--
string(29) "<ns:test xmlns:ns='urn:x'   >"
string(10) "</ns:test>"
