--TEST--
SimpleXML and attributes
--SKIPIF--
<?php if (!extension_loaded("simplexml")) print "skip"; ?>
--FILE--
<?php 

$sxe = simplexml_load_file(dirname(__FILE__).'/sxe.xml');

echo "===Property===\n";
var_dump($sxe->id);
var_dump($sxe->elem1->attr1);
echo "===Array===\n";
var_dump($sxe['id']);
var_dump($sxe->elem1['attr1']);
echo "===Set===\n";
$sxe['id'] = "Changed1";
var_dump($sxe['id']);
$sxe->elem1['attr1'] = 12;
var_dump($sxe->elem1['attr1']);
echo "===Unset===\n";
unset($sxe['id']);
var_dump($sxe['id']);
unset($sxe->elem1['attr1']);
var_dump($sxe->elem1['attr1']);

?>
===Done===
--EXPECT--
===Property===
string(5) "elem1"
string(5) "first"
===Array===
string(5) "elem1"
string(5) "first"
===Set===
string(8) "Changed1"
string(2) "12"
===Unset===
NULL
NULL
===Done===
