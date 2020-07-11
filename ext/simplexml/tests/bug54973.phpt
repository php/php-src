--TEST--
Bug #54973: SimpleXML casts integers wrong
--SKIPIF--
<?php if (!extension_loaded('simplexml')) die('skip simplexml extension not loaded'); ?>
--FILE--
<?php
$xml = simplexml_load_string("<xml><number>9223372036854775808</number></xml>");

var_dump($xml->number);

$int = $xml->number / 1024 / 1024 / 1024;
var_dump($int);

$double = (double) $xml->number / 1024 / 1024 / 1024;
var_dump($double);
?>
--EXPECT--
object(SimpleXMLElement)#2 (1) {
  [0]=>
  string(19) "9223372036854775808"
}
float(8589934592)
float(8589934592)
