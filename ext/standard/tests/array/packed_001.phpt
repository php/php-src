--TEST--
array_keys() and array_values() w/ packed optimization
--FILE--
<?php

$x = [1,2,3];
unset($x[1]);

$inputs = [
  [],
  [1,2,3],
  [0=>1, 1=>2, 2=>3],
  [1=>1, 2=>2, 3=>3],
  [0=>1, 2=>3],
  $x,
];

foreach ($inputs as $input) {
  print_r(array_keys($input));
  print_r(array_values($input));
}
--EXPECT--
Array
(
)
Array
(
)
Array
(
    [0] => 0
    [1] => 1
    [2] => 2
)
Array
(
    [0] => 1
    [1] => 2
    [2] => 3
)
Array
(
    [0] => 0
    [1] => 1
    [2] => 2
)
Array
(
    [0] => 1
    [1] => 2
    [2] => 3
)
Array
(
    [0] => 1
    [1] => 2
    [2] => 3
)
Array
(
    [0] => 1
    [1] => 2
    [2] => 3
)
Array
(
    [0] => 0
    [1] => 2
)
Array
(
    [0] => 1
    [1] => 3
)
Array
(
    [0] => 0
    [1] => 2
)
Array
(
    [0] => 1
    [1] => 3
)
