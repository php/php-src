--TEST--
TokenList: count
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\XMLDocument::createFromString('<root class="a b c"><child/></root>');
$element = $dom->documentElement;
var_dump($element->classList->count(), count($element->classList), $element->classList->length);

?>
--EXPECT--
int(3)
int(3)
int(3)
