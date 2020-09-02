--TEST--
yield from parses too greedily
--FILE--
<?php

function from1234($x) {
  return $x;
}

function bar() {
  yield 24;
}

function foo() {
  yield from1234(42);
  yield from(bar());
}

foreach (foo() as $value) {
  var_dump($value);
}
?>
--EXPECT--
int(42)
int(24)
