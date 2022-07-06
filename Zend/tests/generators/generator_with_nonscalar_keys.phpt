--TEST--
Generators can return non-scalar keys
--FILE--
<?php

function gen() {
    yield [1, 2, 3] => [4, 5, 6];
    yield (object) ['a' => 'b'] => (object) ['b' => 'a'];
    yield 3.14 => 2.73;
    yield false => true;
    yield true => false;
    yield null => null;
}

foreach (gen() as $k => $v) {
    var_dump($k, $v);
}

?>
--EXPECT--
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
array(3) {
  [0]=>
  int(4)
  [1]=>
  int(5)
  [2]=>
  int(6)
}
object(stdClass)#3 (1) {
  ["a"]=>
  string(1) "b"
}
object(stdClass)#4 (1) {
  ["b"]=>
  string(1) "a"
}
float(3.14)
float(2.73)
bool(false)
bool(true)
bool(true)
bool(false)
NULL
NULL
