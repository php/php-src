--TEST--
SimpleXML: Attribute compared to string
--SKIPIF--
<?php if(!extension_loaded("simplexml")) die("skip simplexml extension not loaded"); ?> 
--FILE--
<?php

$doc = simplexml_load_string('<root><name attr="foo">bar</name></root>');
print $doc->name["attr"];
print "\n";
if ($doc->name["attr"] == "foo") {
         print "Works\n";
} else {
         print "Error\n";
}
?>
===DONE===
--EXPECT--
foo
Works
===DONE===
