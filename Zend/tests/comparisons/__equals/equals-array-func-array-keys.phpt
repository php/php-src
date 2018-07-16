--TEST--
__equals: Called by array_keys which has equality semantics (searching)
--FILE--
<?php
include("Equatable.inc");

$a = new Equatable(1);
$b = new Equatable(2);
$c = new Equatable(3);

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

/* Found, because Equatable::__equals returned TRUE */
var_dump(array_keys($array, new Equatable(1), $strict = false)); // a
var_dump(array_keys($array, new Equatable(2), $strict = false)); // b
var_dump(array_keys($array, new Equatable(3), $strict = false)); // c

/* Not found because strict comparison doesn't call __equals */
var_dump(array_keys($array, new Equatable(1), $strict = true)); 
var_dump(array_keys($array, new Equatable(2), $strict = true)); 
var_dump(array_keys($array, new Equatable(3), $strict = true)); 

/* Found, because Equatable::__equals returned TRUE */
var_dump(array_keys($array, 1, $strict = false)); // a
var_dump(array_keys($array, 2, $strict = false)); // b
var_dump(array_keys($array, 3, $strict = false)); // c

/* Not found because strict comparison doesn't call __equals */
var_dump(array_keys($array, 1, $strict = true)); 
var_dump(array_keys($array, 2, $strict = true)); 
var_dump(array_keys($array, 3, $strict = true)); 

/* Not found */
var_dump(array_keys($array, new Equatable(4), $strict = true));
var_dump(array_keys($array, new Equatable(5), $strict = true));

var_dump(array_keys($array, new Equatable(4), $strict = false));
var_dump(array_keys($array, new Equatable(5), $strict = false));

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
