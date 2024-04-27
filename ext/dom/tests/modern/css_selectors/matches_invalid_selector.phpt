--TEST--
Test DOM\Element::matches() method: invalid selector
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\XMLDocument::createFromString("<root/>");

try {
  var_dump($dom->documentElement->matches('@invalid'));
} catch (DOMException $e) {
  echo $e->getMessage();
}

?>
--EXPECT--
Invalid selector (Selectors. Unexpected token: @invalid)
