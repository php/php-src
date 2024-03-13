--TEST--
TokenList: change attribute
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\XMLDocument::createFromString('<root class="a b c"><child/></root>');
$element = $dom->documentElement;
$list = $element->classList;

var_dump($list);

$element->attributes[0]->value = 'd';

var_dump($list);

$list->value = 'e f g';

var_dump($list);

?>
--EXPECT--
object(DOM\TokenList)#3 (2) {
  ["length"]=>
  int(3)
  ["value"]=>
  string(5) "a b c"
}
object(DOM\TokenList)#3 (2) {
  ["length"]=>
  int(1)
  ["value"]=>
  string(1) "d"
}
object(DOM\TokenList)#3 (2) {
  ["length"]=>
  int(3)
  ["value"]=>
  string(5) "e f g"
}
