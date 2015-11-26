--TEST--
SimpleXML [profile]: Accessing a simple node
--SKIPIF--
<?php if (!extension_loaded("simplexml")) print "skip"; ?>
--FILE--
<?php 
$root = simplexml_load_string('<?xml version="1.0"?>
<root>
 <child>Hello</child>
</root>
');

echo $root->child;
echo "\n---Done---\n";
?>
--EXPECT--
Hello
---Done--- 
