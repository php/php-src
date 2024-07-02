--TEST--
TokenList: add
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\XMLDocument::createFromString('<root/>');
$list = $dom->documentElement->classList;

$list->add();
$list->add('a', 'b');
$list->add('c');

$str = 'd';
$ref =& $str;

$list->add($ref);

echo $dom->saveXML(), "\n";

$list->value = '';
$list->add('e');

echo $dom->saveXML(), "\n";

?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<root class="a b c d"/>
<?xml version="1.0" encoding="UTF-8"?>
<root class="e"/>
