--TEST--
XMLReader: Expand Error
--SKIPIF--
<?php if (!extension_loaded("xmlreader")) print "skip";
if (!extension_loaded("dom")) print "skip DOM extension required";
$reader = new XMLReader();
if (!method_exists($reader, 'expand')) print "skip";
?>
--FILE--
<?php

$xmlstring = '<?xml version="1.0" encoding="UTF-8"?>
<books><book>new book</book></books>';

$reader = new XMLReader();
var_dump($reader->expand());
$reader->close();

$reader = new XMLReader();
$reader->XML($xmlstring);
var_dump($reader->expand());
$reader->close();
?>
--EXPECTF--
Warning: XMLReader::expand(): Load Data before trying to expand in %s on line %d
bool(false)

Warning: XMLReader::expand(): An Error Occurred while expanding  in %s on line %d
bool(false)
