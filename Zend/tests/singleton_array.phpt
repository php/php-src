--TEST--
array_init_singleton()
--EXTENSIONS--
zend_test
--FILE--
<?php

$a = zend_new_singleton_array();
var_dump($a, count($a));

/* Stays packed while growing: 1 -> 2 -> 4 -> 8 -> 16 */
$a = zend_new_singleton_array();
for ($i = 0; $i < 10; $i++) {
    $a[] = $i;
}
var_dump($a === range(0, 9));

/* First insert is a string key: goes straight to a mixed table of size 1,
 * whose hash area is only 2 slots wide. */
$a = zend_new_singleton_array();
$a['foo'] = 1;
$a['bar'] = 2;
$a['baz'] = 3;
var_dump($a);

/* Packed -> hash conversion of a size 1 table. */
$a = zend_new_singleton_array();
$a[] = 'first';
$a['key'] = 'second';
var_dump($a);

/* An out of range integer key also skips the packed representation. */
$a = zend_new_singleton_array();
$a[100] = 'hundred';
$a[0] = 'zero';
var_dump($a);

/* Deletion, rehashing and reuse. */
$a = zend_new_singleton_array();
$a['a'] = 1;
$a['b'] = 2;
unset($a['a']);
$a['c'] = 3;
unset($a['b']);
$a['d'] = 4;
var_dump($a);

/* Separation (zend_array_dup) of a size 1 table. */
$a = zend_new_singleton_array();
$a['x'] = 1;
$b = $a;
$b['y'] = 2;
var_dump($a, $b);

/* Sorting and renumbering. */
$a = zend_new_singleton_array();
$a['c'] = 3;
$a['a'] = 1;
$a['b'] = 2;
asort($a);
var_dump($a);
sort($a);
var_dump($a);

/* Round-trips. */
$a = zend_new_singleton_array();
$a['only'] = 'one';
var_dump(unserialize(serialize($a)) === $a);
var_dump(json_decode(json_encode($a), true) === $a);

/* Iteration. */
$a = zend_new_singleton_array();
$a[] = 'a';
$a[] = 'b';
foreach ($a as $k => $v) {
    echo "$k => $v\n";
}

?>
--EXPECT--
array(0) {
}
int(0)
bool(true)
array(3) {
  ["foo"]=>
  int(1)
  ["bar"]=>
  int(2)
  ["baz"]=>
  int(3)
}
array(2) {
  [0]=>
  string(5) "first"
  ["key"]=>
  string(6) "second"
}
array(2) {
  [100]=>
  string(7) "hundred"
  [0]=>
  string(4) "zero"
}
array(2) {
  ["c"]=>
  int(3)
  ["d"]=>
  int(4)
}
array(1) {
  ["x"]=>
  int(1)
}
array(2) {
  ["x"]=>
  int(1)
  ["y"]=>
  int(2)
}
array(3) {
  ["a"]=>
  int(1)
  ["b"]=>
  int(2)
  ["c"]=>
  int(3)
}
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
bool(true)
bool(true)
0 => a
1 => b
