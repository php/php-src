--TEST--
Test DOM\Element::matches() method: invalid selector
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\XMLDocument::createFromString("<root/>");

try {
  var_dump($dom->documentElement->matches('@invalid'));
} catch (Throwable $e) {
  echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
DOMException: Invalid selector (Selectors. Unexpected token: @invalid)
