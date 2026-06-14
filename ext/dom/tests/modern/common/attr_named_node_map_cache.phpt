--TEST--
Attribute named node map cache
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\XMLDocument::createFromString('<root a="1" b="2" c="3"/>');
$attrs = $dom->documentElement->attributes;
var_dump($attrs[1]->nodeName);
$dom->documentElement->removeAttribute('b');
var_dump($attrs[1]->nodeName);

?>
--EXPECT--
string(1) "b"
string(1) "c"
