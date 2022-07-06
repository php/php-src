--TEST--
Test DOMDocument::createAttribute() for expected return value
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$dom = new DOMDocument();

$attr = $dom->createAttribute('string');
echo get_class($attr);

?>
--EXPECT--
DOMAttr
