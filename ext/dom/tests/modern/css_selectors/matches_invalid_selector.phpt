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
  echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
DOMException: Invalid selector (Selectors. Unexpected token: @invalid)
