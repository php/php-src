--TEST--
XMLReader: Expand Error
--EXTENSIONS--
xmlreader
dom
--SKIPIF--
<?php $reader = new XMLReader();
if (!method_exists($reader, 'expand')) print "skip";
?>
--FILE--
<?php

$xmlstring = '<?xml version="1.0" encoding="UTF-8"?>
<books><book>new book</book></books>';

$reader = new XMLReader();

try {
    $reader->expand();
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

$reader->close();

$reader = new XMLReader();
$reader->XML($xmlstring);
var_dump($reader->expand());
$reader->close();
?>
--EXPECTF--
Data must be loaded before expanding

Warning: XMLReader::expand(): An Error Occurred while expanding in %s on line %d
bool(false)
