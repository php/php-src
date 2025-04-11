--TEST--
GH-18304 (Changing the properties of a DateInterval through dynamic properties triggers a SegFault)
--CREDITS--
orose-assetgo
--EXTENSIONS--
dom
--FILE--
<?php
$text = new \DOMText();
$field = 'textContent';
$text->$field .= 'hello';
var_dump($text->$field);
?>
--EXPECT--
string(5) "hello"
