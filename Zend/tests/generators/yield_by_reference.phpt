--TEST--
Generators can yield by-reference
--FILE--
<?php

function *&iter(array &$array) {
    foreach ($array as $key => &$value) {
        yield $key => $value;
    }
}

$array = [1, 2, 3, 4, 5];
$iter = iter($array);
foreach ($iter as &$value) {
    $value *= -1;
}
var_dump($array);

?>
--EXPECT--
array(5) {
  [0]=>
  int(-1)
  [1]=>
  int(-2)
  [2]=>
  int(-3)
  [3]=>
  int(-4)
  [4]=>
  &int(-5)
}
