--TEST--
TokenList: contains
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\XMLDocument::createFromString('<root class="A B C"/>');
$element = $dom->documentElement;
$list = $element->classList;

echo "---\n";

var_dump($list->contains(''));
var_dump($list->contains('A'));
var_dump($list->contains('B'));
var_dump($list->contains('C'));

echo "---\n";

var_dump($list->contains(' A'));
var_dump($list->contains('B '));
var_dump($list->contains(' C '));

echo "---\n";

var_dump($list->contains('a'));
var_dump($list->contains('b'));
var_dump($list->contains('c'));

$element->setAttribute('class', 'D');

echo "---\n";

var_dump($list->contains('A'));
var_dump($list->contains('B'));
var_dump($list->contains('C'));
var_dump($list->contains('D'));

echo "---\n";

$list->value = 'E';
var_dump($list->contains('D'));
var_dump($list->contains('E'));

?>
--EXPECT--
---
bool(false)
bool(true)
bool(true)
bool(true)
---
bool(false)
bool(false)
bool(false)
---
bool(false)
bool(false)
bool(false)
---
bool(false)
bool(false)
bool(false)
bool(true)
---
bool(false)
bool(true)
