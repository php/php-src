--TEST--
Bug #61597: SimpleXMLElement does not respect PHP's array keys cast rule
--SKIPIF--
<?php
if (!extension_loaded('simplexml')) die('skip simplexml extension not available');
?>
--FILE--
<?php
$simpleXMLElement = simplexml_load_string("<xml><number>0</number><number>1</number></xml>");

$int = 1;
$float = 1.0;

$simpleXMLElement->number[$float] = 999;

echo $simpleXMLElement->number[$float] . ', ' . $simpleXMLElement->number[$int];
?>
--EXPECT--
999, 999
