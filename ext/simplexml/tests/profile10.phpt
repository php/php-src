--TEST--
SimpleXML [profile]: Accessing two attributes with the same name, but different namespaces
--SKIPIF--
<?php if (!extension_loaded("simplexml")) print "skip"; ?>
--FILE--
<?php
error_reporting(E_ALL & ~E_NOTICE);
$root = simplexml_load_string('<?xml version="1.0"?>
<root xmlns:reserved="reserved-ns" xmlns:special="special-ns">
 <child reserved:attribute="Sample" special:attribute="Test" />
</root>
');

echo $root->child['reserved:attribute'];
echo "\n";
echo $root->child['special:attribute'];
foreach ($root->child['attribute'] as $attr) {
	echo "$attr\n";
}
echo "\n---Done---\n";
?>
--EXPECT--
Sample
Test
---Done--- 
