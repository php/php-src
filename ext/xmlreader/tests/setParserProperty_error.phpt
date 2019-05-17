--TEST--
XMLReader: setParserProperty Error
--SKIPIF--
<?php if (!extension_loaded("xmlreader")) print "skip XMLReader extension required"; ?>
--FILE--
<?php

$xml = '<?xml version="1.0" encoding="UTF-8"?><books><book>new book</book></books>';

$invalidProperty = -1;
$reader = new XMLReader();
$reader->XML($xml);
var_dump($reader->setParserProperty(-1, true));
$reader->close();
?>
--EXPECTF--
Warning: XMLReader::setParserProperty(): Invalid parser property in %s on line %d
bool(false)
