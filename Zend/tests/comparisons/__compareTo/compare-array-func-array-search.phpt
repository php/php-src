--TEST--
__compareTo: Called by array_search which has equality semantics (searching)
--FILE--
<?php
include("Comparable.inc");

$a = new Comparable(1);
$b = new Comparable(2);
$c = new Comparable(3);

$array = [$a, $b, $c];

var_dump(array_search($a, $array, $strict = false)); // 0
var_dump(array_search($b, $array, $strict = false)); // 1
var_dump(array_search($c, $array, $strict = false)); // 2

var_dump(array_search($a, $array, $strict = true)); // 0
var_dump(array_search($b, $array, $strict = true)); // 1
var_dump(array_search($c, $array, $strict = true)); // 2

/* Found, because Comparable::__compareTo returned 0 */
var_dump(array_search(new Comparable(1), $array, $strict = false)); // 0
var_dump(array_search(new Comparable(2), $array, $strict = false)); // 1
var_dump(array_search(new Comparable(3), $array, $strict = false)); // 2

/* Not found, because not the same instance */
var_dump(array_search(new Comparable(1), $array, $strict = true)); // false
var_dump(array_search(new Comparable(2), $array, $strict = true)); // false
var_dump(array_search(new Comparable(3), $array, $strict = true)); // false

/* Found, because Comparable::__compareTo returned 0 and we haven't implemented __equals */
var_dump(array_search(1, $array, $strict = false)); // 0
var_dump(array_search(2, $array, $strict = false)); // 1
var_dump(array_search(3, $array, $strict = false)); // 2

/* Not found, because strict comparison doesn't call __compareTo */
var_dump(array_search(1, $array, $strict = true)); // false
var_dump(array_search(2, $array, $strict = true)); // false
var_dump(array_search(3, $array, $strict = true)); // false

/* Not found */
var_dump(array_search(new Comparable(4), $array, $strict = false)); // false
var_dump(array_search(new Comparable(5), $array, $strict = false)); // false

var_dump(array_search(new Comparable(4), $array, $strict = true)); // false
var_dump(array_search(new Comparable(5), $array, $strict = true)); // false

?>
--EXPECT--
int(0)
int(1)
int(2)
int(0)
int(1)
int(2)
int(0)
int(1)
int(2)
bool(false)
bool(false)
bool(false)
int(0)
int(1)
int(2)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
