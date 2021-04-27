--TEST--
Bug #80852 (Stack-overflow when json_encode()'ing SimpleXMLElement)
--SKIPIF--
<?php
if (!extension_loaded('simplexml')) die('skip simplexml extension not available');
if (!extension_loaded('json')) die('skip json extension not available');
?>
--FILE--
<?php
$xml = '<!DOCTYPE foo [<!ENTITY xee1 "aaa"> <!ENTITY xee2 "&xee1;&xee1;"> ]><a>b&xee2;</a>';
$sxe = simplexml_load_string($xml);
var_dump(json_encode($sxe));
var_dump($sxe);
?>
--EXPECT--
string(11) "{"xee2":{}}"
object(SimpleXMLElement)#1 (1) {
  ["xee2"]=>
  object(SimpleXMLElement)#3 (0) {
  }
}
