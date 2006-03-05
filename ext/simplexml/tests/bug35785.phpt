--TEST--
Bug #35785 (SimpleXML memory read error)
--FILE--
<?php

$xml = simplexml_load_string("<root></root>");
$xml->bla->posts->name = "FooBar";
echo $xml->asXML();

echo "===FAIL===\n";

$xml = simplexml_load_string("<root></root>");
$count = count($xml->bla->posts);
var_dump($count);
$xml->bla->posts[++$count]->name = "FooBar";
echo $xml->asXML();
?>
===DONE===
<?php exit(0); __halt_compiler(); ?>
--EXPECTF--
<?xml version="1.0"?>
<root><bla><posts><name>FooBar</name></posts></bla></root>
===FAIL===
int(0)

Fatal error: Objects used as arrays in post/pre increment/decrement must return values by reference in %sbug35785.php on line %d
