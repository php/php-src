--TEST--
TokenList: replace
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\XMLDocument::createFromString('<root class="A B C"/>');
$element = $dom->documentElement;
$list = $element->classList;

var_dump($list->replace('nonexistent', 'X'));

echo $dom->saveXML(), "\n";

var_dump($list->replace('B', 'X'));

echo $dom->saveXML(), "\n";

var_dump($list->replace('C', 'X'));

echo $dom->saveXML(), "\n";

var_dump($list->replace('A', 'B'));

echo $dom->saveXML(), "\n";

var_dump($list->replace('X', 'B'));

echo $dom->saveXML(), "\n";

$list->value = 'A';
$list->replace('A', 'AA');

echo $dom->saveXML(), "\n";

?>
--EXPECT--
bool(false)
<?xml version="1.0" encoding="UTF-8"?>
<root class="A B C"/>
bool(true)
<?xml version="1.0" encoding="UTF-8"?>
<root class="A X C"/>
bool(true)
<?xml version="1.0" encoding="UTF-8"?>
<root class="A X"/>
bool(true)
<?xml version="1.0" encoding="UTF-8"?>
<root class="B X"/>
bool(true)
<?xml version="1.0" encoding="UTF-8"?>
<root class="B"/>
<?xml version="1.0" encoding="UTF-8"?>
<root class="AA"/>
