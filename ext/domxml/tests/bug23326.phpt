--TEST--
Attributes via append_child not supported (bug #23326)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$document = domxml_new_doc('1.0');
$test = $document->create_element('test');
$j = 0;
for($i=0;$i<10;$i++)
{
	if( $j < 5 )
		$j = $i;
	$test->append_child($document->create_attribute("key$j", $i));
}
$document->append_child($test);
echo $document->dump_mem(1);
?>
--EXPECT--
<?xml version="1.0"?>
<test key0="0" key1="1" key2="2" key3="3" key4="4" key5="9"/>
