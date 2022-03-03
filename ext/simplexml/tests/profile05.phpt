--TEST--
SimpleXML [profile]: Accessing an aliased namespaced element
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

echo $root->children('reserved')->child;
echo "\n---Done---\n";
?>
--EXPECT--
---Done--- 
