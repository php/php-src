--TEST--
TokenList: clone
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\XMLDocument::createFromString('<root class="a b c"><child/></root>');
$element = $dom->documentElement;
clone $element->classList;

?>
--EXPECTF--
Fatal error: Uncaught Error: Trying to clone an uncloneable object of class DOM\TokenList in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
