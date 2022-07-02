--TEST--
SimpleXML [profile]: Accessing an attribute
--EXTENSIONS--
simplexml
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
