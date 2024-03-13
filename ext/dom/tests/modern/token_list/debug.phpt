--TEST--
TokenList: debug
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\XMLDocument::createFromString('<root class="a b c"><child/></root>');
$element = $dom->documentElement;
var_dump($element->classList);

?>
--EXPECT--
object(DOM\TokenList)#3 (2) {
  ["length"]=>
  int(3)
  ["value"]=>
  string(5) "a b c"
}
