--TEST--
"Reference Unpacking - VM Safety" list()
--FILE--
<?php
$ary = [[0, 1]];
[[
    0 => &$a,
    ($ary["foo"] = 1) => &$b
]] = $ary;

var_dump($ary, $a, $b);
unset($ary, $a, $b);

$ary = [[0, 1]];
[
    0 => &$a,
    ($ary["foo"] = 1) => &$b
] = $ary[0];
var_dump($ary, $a, $b);
?>
--EXPECT--
array(2) {
  [0]=>
  array(2) {
    [0]=>
    &int(0)
    [1]=>
    &int(1)
  }
  ["foo"]=>
  int(1)
}
int(0)
int(1)
array(2) {
  [0]=>
  array(2) {
    [0]=>
    &int(0)
    [1]=>
    &int(1)
  }
  ["foo"]=>
  int(1)
}
int(0)
int(1)
