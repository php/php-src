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
var_dump(isset($xml->bla->posts));
$xml->bla->posts[0]->name = "FooBar";
echo $xml->asXML();
$xml = simplexml_load_string("<root></root>");
$xml->bla->posts[]->name = "FooBar";
echo $xml->asXML();
?>
--EXPECT--
<?xml version="1.0"?>
<root><bla><posts><name>FooBar</name></posts></bla></root>
bool(false)
<?xml version="1.0"?>
<root><bla><posts><name>FooBar</name></posts></bla></root>
<?xml version="1.0"?>
<root><bla><posts><name>FooBar</name></posts></bla></root>
