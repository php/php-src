--TEST--
SimpleXML [profile]: Accessing two elements with the same name, but different namespaces
--SKIPIF--
<?php if (!extension_loaded("simplexml")) print "skip"; ?>
--FILE--
<?php
error_reporting(E_ALL & ~E_NOTICE);
$root = simplexml_load_string(b'<?xml version="1.0"?>
<root xmlns:reserved="reserved-ns" xmlns:special="special-ns">
 <reserved:child>Hello</reserved:child>
 <special:child>World</special:child>
</root>
');

var_dump($root->children('reserved-ns')->child);
var_dump($root->children('special-ns')->child);
var_dump((string)$root->children('reserved-ns')->child);
var_dump((string)$root->children('special-ns')->child);
var_dump($root->child);
?>
===DONE===
--EXPECTF--
object(SimpleXMLElement)#%d (1) {
  [0]=>
  unicode(5) "Hello"
}
object(SimpleXMLElement)#%d (1) {
  [0]=>
  unicode(5) "World"
}
unicode(5) "Hello"
unicode(5) "World"
object(SimpleXMLElement)#%d (0) {
}
===DONE===
