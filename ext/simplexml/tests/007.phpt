--TEST--
SimpleXML and attributes
--SKIPIF--
<?php if (!extension_loaded("simplexml")) print "skip"; ?>
--FILE--
<?php 

$sxe = simplexml_load_file(dirname(__FILE__).'/sxe.xml');

var_dump($sxe->id);
var_dump($sxe->elem1->attr1);

echo "===Done===\n";

?>
--EXPECT--
string(5) "elem1"
string(5) "first"
===Done===
