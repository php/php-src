--TEST--
Bug #79922 (Crash after multiple calls to xml_parser_free())
--SKIPIF--
<?php
if (!extension_loaded('xml')) die('skip xml extension not available');
?>
--FILE--
<?php
$c=xml_parser_create_ns();
$a=xml_parser_free($c);
$a=xml_parser_free($c);
$c=0;
var_dump($a);
?>
--EXPECTF--
Warning: xml_parser_free(): supplied resource is not a valid XML Parser resource in %s on line %d
bool(false)
