--TEST--
SPL: Test that __unserialize converts references to non-references
--FILE--
<?php

$s = new SplObjectStorage();
$y = 1;
$o = new stdClass();
$x = [$o, &$y];
$s->__unserialize([$x, []]);
var_dump($s);
$val = $s[$o];
$val = 123;
var_dump($s);
?>
--EXPECT--
object(SplObjectStorage)#1 (1) {
  ["storage":"SplObjectStorage":private]=>
  array(1) {
    [0]=>
    array(2) {
      ["obj"]=>
      object(stdClass)#2 (0) {
      }
      ["inf"]=>
      int(1)
    }
  }
}
object(SplObjectStorage)#1 (1) {
  ["storage":"SplObjectStorage":private]=>
  array(1) {
    [0]=>
    array(2) {
      ["obj"]=>
      object(stdClass)#2 (0) {
      }
      ["inf"]=>
      int(1)
    }
  }
}
