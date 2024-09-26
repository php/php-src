--TEST--
Virtual property existence tests
--EXTENSIONS--
xmlreader
--FILE--
<?php

$di = new XMLReader();

var_dump(isset($di->attributeCount));
var_dump(empty($di->attributeCount));
var_dump(property_exists($di, "attributeCount"));

var_dump(isset($di->baseURI));
var_dump(empty($di->baseURI));
var_dump(property_exists($di, "baseURI"));

var_dump(isset($di->depth));
var_dump(empty($di->depth));
var_dump(property_exists($di, "depth"));

var_dump(isset($di->hasAttributes));
var_dump(empty($di->hasAttributes));
var_dump(property_exists($di, "hasAttributes"));

var_dump(isset($di->hasValue));
var_dump(empty($di->hasValue));
var_dump(property_exists($di, "hasValue"));

?>
--EXPECTF--
bool(true)
bool(false)
bool(true)
bool(true)
bool(false)
bool(true)
bool(true)
bool(false)
bool(true)
bool(true)
bool(false)
bool(true)
bool(true)
bool(false)
bool(true)
