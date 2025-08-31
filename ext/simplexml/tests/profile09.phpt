--TEST--
SimpleXML [profile]: Accessing a namespaced element without a namespace
--EXTENSIONS--
simplexml
--FILE--
<?php
error_reporting(E_ALL & ~E_NOTICE);
$root = simplexml_load_string('<?xml version="1.0"?>
<root xmlns:reserved="reserved-ns">
 <reserved:child>Hello</reserved:child>
</root>
');

echo $root->child;
echo "\n---Done---\n";
?>
--EXPECT--
---Done--- 
