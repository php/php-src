--TEST--
Bug #72099: xml_parse_into_struct segmentation fault
--SKIPIF--
<?php
require_once("skipif.inc");
?>
--FILE--
<?php
$var1=xml_parser_create_ns();
$var2=str_repeat("a", 10);
$var3=[];
$var4=[];
xml_parse_into_struct($var1, $var2, $var3, $var4);
var_dump($var3);
--EXPECT--
array(0) {
}
