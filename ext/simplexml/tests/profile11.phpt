--TEST--
SimpleXML [profile]: Accessing two elements with the same name, but different namespaces
--SKIPIF--
<?php if (!extension_loaded("simplexml")) print "skip"; ?>
--FILE--
<?php
error_reporting(E_ALL & ~E_NOTICE);
$root = simplexml_load_string('<?xml version="1.0"?>
<root xmlns:reserved="reserved-ns" xmlns:special="special-ns">
 <reserved:child>Hello</reserved:child>
 <special:child>World</special:child>
</root>
');

echo $root->children('reserved-ns')->child;
echo "\n";
echo $root->children('special-ns')->child;
foreach ($root->child as $child) {
	echo "$child\n";
}
echo "\n---Done---\n";
?>
--EXPECT--
Hello
World
---Done--- 
