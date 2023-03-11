--TEST--
Bug #80332 (Completely broken array access functionality with DOMNamedNodeMap)
--EXTENSIONS--
dom
--FILE--
<?php
$doc = new DOMDocument;
$doc->loadHTML('<span attr1="value1" attr2="value2"></span>');

$x = new DOMXPath($doc);
$span = $x->query('//span')[0];

var_dump($span->attributes[0]->nodeName);
var_dump($span->attributes[0]->nodeValue);

var_dump($span->attributes['1']->nodeName);
var_dump($span->attributes['1']->nodeValue);

var_dump($span->attributes[0.6]->nodeName);
var_dump($span->attributes[0.6]->nodeValue);

var_dump($span->attributes['0.6']->nodeName);
var_dump($span->attributes['0.6']->nodeValue);

var_dump($span->attributes[12345678]->nodeName);
var_dump($span->attributes[12345678]->nodeValue);

var_dump($span->attributes['12345678']->nodeName);
var_dump($span->attributes['12345678']->nodeValue);

var_dump($span->attributes[9999999999999999999999999999999999]->nodeName);
var_dump($span->attributes[9999999999999999999999999999999999]->nodeValue);

var_dump($span->attributes['9999999999999999999999999999999999']->nodeName);
var_dump($span->attributes['9999999999999999999999999999999999']->nodeValue);

var_dump($span->attributes['hi']->nodeName);
var_dump($span->attributes['hi']->nodeValue);
?>
--EXPECTF--
string(5) "attr1"
string(6) "value1"
string(5) "attr2"
string(6) "value2"

Warning: Attempt to read property "nodeName" on null in %s on line %d
NULL

Warning: Attempt to read property "nodeValue" on null in %s on line %d
NULL

Warning: Attempt to read property "nodeName" on null in %s on line %d
NULL

Warning: Attempt to read property "nodeValue" on null in %s on line %d
NULL

Warning: Attempt to read property "nodeName" on null in %s on line %d
NULL

Warning: Attempt to read property "nodeValue" on null in %s on line %d
NULL

Warning: Attempt to read property "nodeName" on null in %s on line %d
NULL

Warning: Attempt to read property "nodeValue" on null in %s on line %d
NULL

Warning: Attempt to read property "nodeName" on null in %s on line %d
NULL

Warning: Attempt to read property "nodeValue" on null in %s on line %d
NULL

Warning: Attempt to read property "nodeName" on null in %s on line %d
NULL

Warning: Attempt to read property "nodeValue" on null in %s on line %d
NULL

Warning: Attempt to read property "nodeName" on null in %s on line %d
NULL

Warning: Attempt to read property "nodeValue" on null in %s on line %d
NULL
