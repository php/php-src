--TEST--
SimpleXML [profile]: Accessing a namespaced attribute
--EXTENSIONS--
simplexml
--FILE--
<?php
error_reporting(E_ALL & ~E_NOTICE);
$root = simplexml_load_string('<?xml version="1.0"?>
<root xmlns:reserved="reserved-ns">
 <child reserved:attribute="Sample" />
</root>
');

$attr = $root->child->attributes('reserved-ns');
echo $attr['attribute'];
echo "\n---Done---\n";
?>
--EXPECT--
Sample
---Done--- 
