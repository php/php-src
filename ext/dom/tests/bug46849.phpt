--TEST--
Bug #46849 (Cloning DOMDocument doesn't clone the properties).
--EXTENSIONS--
dom
--FILE--
<?php
$dom = new DOMDocument;
$dom->formatOutput = 1;
var_dump($dom->formatOutput);

$dom2 = clone $dom;
var_dump($dom2->formatOutput);
?>
--EXPECT--
bool(true)
bool(true)
