--TEST--
array_map(): dynamic callable falls back to native semantics
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
--FILE--
<?php

function plus1($x) {
    return $x + 1;
}

$array = range(1, 10);

$plus1 = 'plus1';
$foo = array_map($plus1(...), $array);

var_dump($foo);

?>
--EXPECT--
array(10) {
  [0]=>
  int(2)
  [1]=>
  int(3)
  [2]=>
  int(4)
  [3]=>
  int(5)
  [4]=>
  int(6)
  [5]=>
  int(7)
  [6]=>
  int(8)
  [7]=>
  int(9)
  [8]=>
  int(10)
  [9]=>
  int(11)
}
