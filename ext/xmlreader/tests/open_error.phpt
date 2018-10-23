--TEST--
XMLReader: open error
--SKIPIF--
<?php if (!extension_loaded("xmlreader")) print "skip XMLReader extension required"; ?>
--FILE--
<?php
$reader = new XMLReader();
var_dump($reader->open(__DIR__.'/missing-file.xml'));
$reader->close();
?>
--EXPECTF--
Warning: XMLReader::open(): Unable to open source data in %s on line %d
bool(false)
