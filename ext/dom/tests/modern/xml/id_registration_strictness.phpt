--TEST--
Strictness of ID registration
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\XMLDocument::createFromString('<root><child0 xml:id="a"/><child1 id="a"/><child2 id="b"/></root>');
var_dump($dom->getElementById('a')?->tagName);
var_dump($dom->getElementById('b')?->tagName);
$dom->documentElement->setAttribute('id', 'a');
var_dump($dom->getElementById('a')?->tagName);
$dom->documentElement->setAttribute('id', '');
var_dump($dom->getElementById('a')?->tagName);
$dom->documentElement->setAttribute('ID', 'a');
var_dump($dom->getElementById('a')?->tagName);

?>
--EXPECT--
string(6) "child1"
string(6) "child2"
string(4) "root"
string(6) "child1"
string(6) "child1"
