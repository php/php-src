--TEST--
__compareTo: Called by array_keys which has equality semantics (searching)
--FILE--
<?php
include("Comparable.inc");

$a = new Comparable(1);
$b = new Comparable(2);
$c = new Comparable(3);

$array = [
    'a' => $a, 
    'b' => $b, 
    'c' => $c,
];

var_dump(array_keys($array, $a, $strict = false)); // a
var_dump(array_keys($array, $b, $strict = false)); // b
var_dump(array_keys($array, $c, $strict = false)); // c

var_dump(array_keys($array, $a, $strict = true)); // a
var_dump(array_keys($array, $b, $strict = true)); // b
var_dump(array_keys($array, $c, $strict = true)); // c

/* Found, because Comparable::__compareTo returned 0 */
var_dump(array_keys($array, new Comparable(1), $strict = false)); // a
var_dump(array_keys($array, new Comparable(2), $strict = false)); // b
var_dump(array_keys($array, new Comparable(3), $strict = false)); // c

/* Not found because strict comparison doesn't call __compareTo */
var_dump(array_keys($array, new Comparable(1), $strict = true)); 
var_dump(array_keys($array, new Comparable(2), $strict = true)); 
var_dump(array_keys($array, new Comparable(3), $strict = true)); 

/* Found, because Comparable::__compareTo returned 0 and we haven't implemented __equals */
var_dump(array_keys($array, 1, $strict = false)); // a
var_dump(array_keys($array, 2, $strict = false)); // b
var_dump(array_keys($array, 3, $strict = false)); // c

/* Not found because strict comparison doesn't call __compareTo */
var_dump(array_keys($array, 1, $strict = true)); 
var_dump(array_keys($array, 2, $strict = true)); 
var_dump(array_keys($array, 3, $strict = true)); 

/* Not found */
var_dump(array_keys($array, new Comparable(4), $strict = true));
var_dump(array_keys($array, new Comparable(5), $strict = true));

var_dump(array_keys($array, new Comparable(4), $strict = false));
var_dump(array_keys($array, new Comparable(5), $strict = false));

?>
--EXPECT--
array(1) {
  [0]=>
  string(1) "a"
}
array(1) {
  [0]=>
  string(1) "b"
}
array(1) {
  [0]=>
  string(1) "c"
}
array(1) {
  [0]=>
  string(1) "a"
}
array(1) {
  [0]=>
  string(1) "b"
}
array(1) {
  [0]=>
  string(1) "c"
}
array(1) {
  [0]=>
  string(1) "a"
}
array(1) {
  [0]=>
  string(1) "b"
}
array(1) {
  [0]=>
  string(1) "c"
}
array(0) {
}
array(0) {
}
array(0) {
}
array(1) {
  [0]=>
  string(1) "a"
}
array(1) {
  [0]=>
  string(1) "b"
}
array(1) {
  [0]=>
  string(1) "c"
}
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
