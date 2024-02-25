--TEST--
DOMNamedNodeMap string references
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOMDocument;
$dom->loadXML('<a href="hi" foo="bar"/>');

$attributes = $dom->documentElement->attributes;

var_dump(isset($attributes['href']), $attributes['href']->value);

var_dump(isset($attributes['foo']), $attributes['foo']->value);

$str = 'href';
$ref =& $str;
var_dump(isset($attributes[$ref]), $attributes[$ref]->value);

$str = 'foo';
$ref =& $str;
var_dump(isset($attributes[$ref]), $attributes[$ref]->value);

$str = 'this does not exist';
$ref =& $str;
var_dump(isset($attributes[$ref]), $attributes[$ref]->value);

$str = '0';
$ref =& $str;
var_dump(isset($attributes[$ref]), $attributes[$ref]->value);

$str = '1';
$ref =& $str;
var_dump(isset($attributes[$ref]), $attributes[$ref]->value);

?>
--EXPECT--
bool(true)
string(2) "hi"
bool(true)
string(3) "bar"
bool(true)
string(2) "hi"
bool(true)
string(2) "hi"
bool(true)
string(2) "hi"
bool(true)
string(2) "hi"
bool(true)
string(3) "bar"
