--TEST--
Crash in childNodes iterator current()
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOMDocument;
$dom->loadXML('<foo>foo1</foo>');

$nodes = $dom->documentElement->childNodes;
$iter = $nodes->getIterator();

var_dump($iter->valid());
var_dump($iter->current()?->wholeText);
$iter->next();
var_dump($iter->valid());
var_dump($iter->current()?->wholeText);

?>
--EXPECT--
bool(true)
string(4) "foo1"
bool(false)
NULL
