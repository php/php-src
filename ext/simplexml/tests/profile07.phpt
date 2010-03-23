--TEST--
SimpleXML [profile]: Accessing an aliased namespaced attribute
--SKIPIF--
<?php if (!extension_loaded("simplexml")) print "skip"; ?>
--FILE--
<?php
error_reporting(E_ALL & ~E_NOTICE);
$root = simplexml_load_string('<?xml version="1.0"?>
<root xmlns:reserved="reserved-ns">
 <child reserved:attribute="Sample" />
</root>
');

$rsattr = $root->child->attributes('reserved');
$myattr = $root->child->attributes('reserved-ns');

echo $rsattr['attribute'];
echo $myattr['attribute'];
echo "\n---Done---\n";
?>
--EXPECT--
Sample
---Done--- 
