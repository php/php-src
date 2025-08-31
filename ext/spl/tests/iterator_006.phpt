--TEST--
SPL: IteratorIterator and SimpleXMlElement
--EXTENSIONS--
simplexml
--FILE--
<?php

$root = simplexml_load_string('<?xml version="1.0"?>
<root>
 <child>Hello</child>
 <child>World</child>
</root>
');

foreach (new IteratorIterator($root->child) as $child) {
    echo $child."\n";
}
?>
--EXPECT--
Hello
World
