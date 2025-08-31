--TEST--
Named args after unpacking (supported)
--FILE--
<?php

function test(...$args) {
    var_dump($args);
}

test(...[1, 2], a: 3);
test(...[1, 'a' => 2], b: 3);

function test2($a, $b, $c = 3, $d = 4) {
    var_dump($a, $b, $c, $d);
}

test2(...[1, 2], d: 40);
test2(...['b' => 2, 'a' => 1], d: 40);

try {
    test2(...[1, 2], b: 20);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    test2(...[1, 'b' => 2], b: 20);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  ["a"]=>
  int(3)
}
array(3) {
  [0]=>
  int(1)
  ["a"]=>
  int(2)
  ["b"]=>
  int(3)
}
int(1)
int(2)
int(3)
int(40)
int(1)
int(2)
int(3)
int(40)
Named parameter $b overwrites previous argument
Named parameter $b overwrites previous argument
