--TEST--
SimpleXML [profile]: Accessing an aliased namespaced element
--SKIPIF--
<?php if (!extension_loaded("simplexml")) print "skip"; ?>
--FILE--
<?php
error_reporting(E_ALL & ~E_NOTICE);
$root = simplexml_load_string('<?xml version="1.0"?>
<root xmlns:reserved="reserved-ns">
 <reserved:child>Hello</reserved:child>
</root>
');

$root->register_ns('myns', 'reserved-ns');

echo $root->myns->child;
echo $root->reserved->child;
echo "\n---Done---\n";
?>
--EXPECT--
Hello
---Done--- 
