--TEST--
TokenList: remove
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\XMLDocument::createFromString('<root class="test1    test2"/>');
$list = $dom->documentElement->classList;

$list->remove();
$list->remove('test1');

echo $dom->saveXml(), "\n";

$list->remove('nope');

echo $dom->saveXml(), "\n";

$list->remove('test2');

echo $dom->saveXml(), "\n";

$list->value = 'test3 test4';
$list->remove('test4');

echo $dom->saveXml(), "\n";

?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<root class="test2"/>
<?xml version="1.0" encoding="UTF-8"?>
<root class="test2"/>
<?xml version="1.0" encoding="UTF-8"?>
<root class=""/>
<?xml version="1.0" encoding="UTF-8"?>
<root class="test3"/>
