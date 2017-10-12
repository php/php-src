--TEST--
XMLReader: open variant static call
--SKIPIF--
<?php if (!extension_loaded("xmlreader")) print "skip XMLReader extension required"; ?>
--FILE--
<?php

$reader = XMLReader::open(__DIR__ . '/012.xml');
var_dump(is_object($reader));
?>
--EXPECTF--
Deprecated: Non-static method XMLReader::open() should not be called statically in %s on line %d
bool(true)
