--TEST--
Test DOM\Element::closest() method: invalid selector
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\XMLDocument::createFromString("<root/>");

try {
  var_dump($dom->documentElement->closest('@invalid'));
} catch (DOMException $e) {
  echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
DOMException: Invalid selector (Selectors. Unexpected token: @invalid)
