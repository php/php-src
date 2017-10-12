--TEST--
XMLReader: XML Error
--SKIPIF--
<?php if (!extension_loaded("xmlreader")) print "skip XMLReader extension required"; ?>
--FILE--
<?php

$reader = new XMLReader();
var_dump($reader->XML());
?>
--EXPECTF--
Warning: XMLReader::XML() expects at least 1 parameter, 0 given in %s on line %d
NULL
