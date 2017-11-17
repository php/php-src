--TEST--
SPL: IteratorIterator and SimpleXMlElement
--SKIPIF--
<?php if (!extension_loaded('simplexml')) print "skip SimpleXML required"; ?>
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
===DONE===
<?php exit(0); ?>
--EXPECT--
Hello
World
===DONE===
