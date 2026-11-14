--TEST--
array_merge() with an empty argument returns an array with a fresh next free index
--FILE--
<?php
// The result must behave like a fresh array: integer keys renumbered from 0.

$a = [1, 2, 3];
unset($a[2]);
$r = array_merge($a, []);
$r[] = 4;
var_dump($r);

$b = ['a' => 1];
$b[] = 2;
unset($b[0]);
$r = array_merge([], $b);
$r[] = 3;
var_dump($r);

// A temporary array is reused in place when it is the first argument.
function holey_array() {
	$a = [1, 2, 3];
	unset($a[2]);
	return $a;
}

$r = array_merge(holey_array(), []);
$r[] = 4;
var_dump($r);

$r = array_merge(holey_array());
$r[] = 4;
var_dump($r);

$r = array_merge(holey_array(), ['k' => 'x']);
$r[] = 4;
var_dump($r);

$r = array_merge_recursive(holey_array(), []);
$r[] = 4;
var_dump($r);
?>
--EXPECT--
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(4)
}
array(2) {
  ["a"]=>
  int(1)
  [0]=>
  int(3)
}
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(4)
}
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(4)
}
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  ["k"]=>
  string(1) "x"
  [2]=>
  int(4)
}
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(4)
}
