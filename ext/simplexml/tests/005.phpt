--TEST--
SimpleXML and text data
--SKIPIF--
<?php if (!extension_loaded("simplexml")) print "skip"; ?>
--FILE--
<?php 

$sxe = simplexml_load_file(dirname(__FILE__).'/005.xml');

var_dump(trim($sxe->elem1->elem2));
var_dump(trim($sxe->elem1->elem2->elem3));
var_dump(trim($sxe->elem1->elem2->elem3->elem4));

echo "---Done---\n";

?>
--EXPECT--
string(28) "Here we have some text data."
string(19) "And here some more."
string(15) "Wow once again."
---Done--- 
