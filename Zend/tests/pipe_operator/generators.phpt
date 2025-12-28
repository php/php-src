--TEST--
Generators
--FILE--
<?php

function producer(): \Generator {
  yield 1;
  yield 2;
  yield 3;
}

function map_incr(iterable $it): \Generator {
  foreach ($it as $val) {
    yield $val +1;
  }
}

$result = producer() |> map_incr(...) |> iterator_to_array(...);

var_dump($result);
?>
--EXPECT--
array(3) {
  [0]=>
  int(2)
  [1]=>
  int(3)
  [2]=>
  int(4)
}
