--TEST--
Bug #35785 (SimpleXML memory read error)
--SKIPIF--
<?php if (!extension_loaded("simplexml")) print "skip"; ?>
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

Notice: Indirect modification of overloaded element of SimpleXMLElement has no effect in %sbug35785.php on line %d

Strict Standards: Creating default object from empty value in %sbug35785.php on line %d
<?xml version="1.0"?>
<root><bla><posts/></bla></root>
===DONE===
