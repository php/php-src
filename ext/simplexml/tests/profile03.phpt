--TEST--
SimpleXML [profile]: Accessing an attribute
--SKIPIF--
<?php if (!extension_loaded("simplexml")) print "skip"; ?>
--FILE--
<?php 
$root = simplexml_load_string('<?xml version="1.0"?>
<root>
 <child attribute="Sample" />
</root>
');

echo $root->child['attribute'];
echo "\n---Done---\n";
?>
--EXPECT--
Sample
---Done--- 
