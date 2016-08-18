--TEST--
Bug #72116 (insertion after array_fill fails)
--FILE--
<?php

$x = array_fill(0, 1, '..');
$x[] = 'a';
var_dump($x);

?>
--EXPECT--
array(2) {
  [0]=>
  string(2) ".."
  [1]=>
  string(1) "a"
}

