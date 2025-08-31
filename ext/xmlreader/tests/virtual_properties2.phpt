--TEST--
Virtual property existence tests
--EXTENSIONS--
xmlreader
--FILE--
<?php

$reader = new XMLReader();

var_dump(isset($reader->attributeCount));
var_dump(empty($reader->attributeCount));
var_dump(property_exists($reader, "attributeCount"));

var_dump(isset($reader->baseURI));
var_dump(empty($reader->baseURI));
var_dump(property_exists($reader, "baseURI"));

var_dump(isset($reader->depth));
var_dump(empty($reader->depth));
var_dump(property_exists($reader, "depth"));

var_dump(isset($reader->hasAttributes));
var_dump(empty($reader->hasAttributes));
var_dump(property_exists($reader, "hasAttributes"));

var_dump(isset($reader->hasValue));
var_dump(empty($reader->hasValue));
var_dump(property_exists($reader, "hasValue"));

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
