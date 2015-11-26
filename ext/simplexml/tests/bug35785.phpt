--TEST--
Bug #35785 (SimpleXML memory read error)
--SKIPIF--
<?php if (!extension_loaded("simplexml")) print "skip"; ?>
--FILE--
<?php

$xml = simplexml_load_string("<root></root>");
$xml->bla->posts->name = "FooBar";
echo $xml->asXML();
$xml = simplexml_load_string("<root></root>");
$count = count($xml->bla->posts);
var_dump($count);
$xml->bla->posts[$count]->name = "FooBar";
echo $xml->asXML();
$xml = simplexml_load_string("<root></root>");
$xml->bla->posts[]->name = "FooBar";
echo $xml->asXML();
?>
===DONE===
<?php exit(0); __halt_compiler(); ?>
--EXPECTF--
<?xml version="1.0"?>
<root><bla><posts><name>FooBar</name></posts></bla></root>
int(0)
<?xml version="1.0"?>
<root><bla><posts><name>FooBar</name></posts></bla></root>
<?xml version="1.0"?>
<root><bla><posts><name>FooBar</name></posts></bla></root>
===DONE===
