--TEST--
Testing getDocNamespaces() with invalid XML
--SKIPIF--
<?php if (!extension_loaded('simplexml')) die('skip simplexml extension not loaded'); ?>
--FILE--
<?php
$xml = @new SimpleXMLElement("X",1);
var_dump($xml->getDocNamespaces());
?>
--EXPECT--
bool(false)
