--TEST--
Test seek() function : basic functionality
--FILE--
<?php
/* Prototype  : bool seek(array $array_arg, $offset, $whence = SEEK_CUR)
 * Description: Set array argument's internal pointer to given offset
 * Source code: ext/standard/array.c
 */

/*
 * Test basic functionality of reset()
 */

echo "*** Testing seek() : arguments ***\n";

$array = array('zero', 'one', 200 => 'two');
$extra = null;

var_dump(seek());
var_dump(seek($array));
var_dump(seek($array, 0, SEEK_CUR, $extra));

echo "\n*** Testing seek() : basic functionality ***\n";

var_dump(seek($array, 1, SEEK_SET));
var_dump(current($array));

var_dump(seek($array, 1));
var_dump(current($array));

var_dump(seek($array, -1));
var_dump(current($array));

var_dump(seek($array, -1, SEEK_END));
var_dump(current($array));

echo "\n*** Testing seek() : edge cases ***\n";

var_dump(seek($array, count($array), SEEK_SET));
var_dump(seek($array, -1, SEEK_SET));
var_dump(current($array));
var_dump(seek($array, -count($array) - 1, SEEK_END));
var_dump(seek($array, 1, SEEK_END));
var_dump(current($array));

echo "\n*** Testing seek() : using string keys ***\n";

$array = array('zero', 'one' => 'foo', 'bar');
var_dump(seek($array, 'one', SEEK_KEY));
var_dump(current($array));

?>
===DONE===
--EXPECTF--
*** Testing seek() : arguments ***

Warning: seek() expects at least 2 parameters, 0 given in %s on line %d
NULL

Warning: seek() expects at least 2 parameters, 1 given in %s on line %d
NULL

Warning: seek() expects at most 3 parameters, 4 given in %s on line %d
NULL

*** Testing seek() : basic functionality ***
bool(true)
string(%d) "one"
bool(true)
string(%d) "two"
bool(true)
string(%d) "one"
bool(true)
string(%d) "two"

*** Testing seek() : edge cases ***
bool(false)
bool(false)
string(%d) "two"
bool(false)
bool(false)
string(%d) "two"

*** Testing seek() : using string keys ***
bool(true)
string(3) "foo"
===DONE===
