--TEST--
Bug #37076 (SimpleXML ignores .=)
--SKIPIF--
<?php if (!extension_loaded("simplexml")) print "skip"; ?>
--FILE--
<?php
$xml = simplexml_load_string("<root><foo /></root>");
$xml->foo = "foo";
$xml->foo .= "bar";
print $xml->asXML();
?>
--EXPECT--
<?xml version="1.0"?>
<root><foo>foobar</foo></root>
