--TEST--
Test DOM\Element::closest() method: invalid selector
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\XMLDocument::createFromString("<root/>");

try {
  var_dump($dom->documentElement->closest('@invalid'));
} catch (Throwable $e) {
  echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
DOMException: Invalid selector (Selectors. Unexpected token: @invalid)
