--TEST--
TokenList: contains empty
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\XMLDocument::createFromString('<root/>');
$element = $dom->documentElement;
$list = $element->classList;

var_dump($list->contains('x'));

?>
--EXPECT--
bool(false)
