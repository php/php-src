--TEST--
SimpleXML: implement Countable
--SKIPIF--
<?php if (!extension_loaded("simplexml")) print "skip"; ?>
--FILE--
<?php

$str = '<xml></xml>';
$sxe = new SimpleXmlElement($str);
var_dump($sxe instanceof Countable);
?>
--EXPECT--
bool(true)
