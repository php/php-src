--TEST--
Test count() function : null as argument
--FILE--
<?php
/**
 * Prototype  : int count(array|Countable|null $var [, int $mode])
 * Description: Count the number of elements in a variable (usually an array)
 * Source code: ext/standard/array.c
 */

var_dump(count(NULL));
var_dump(count(NULL, COUNT_NORMAL));
var_dump(count(NULL, COUNT_RECURSIVE));

$var;
var_dump(count($var));
var_dump(count($var, COUNT_NORMAL));
var_dump(count($var, COUNT_RECURSIVE));

$array = [];
var_dump(count($array['key']));
var_dump(count($array['key'], COUNT_NORMAL));
var_dump(count($array['key'], COUNT_RECURSIVE));

?>
--EXPECTF--
Deprecated: count(): Passing null is deprecated in %s on line %d
int(0)

Deprecated: count(): Passing null is deprecated in %s on line %d
int(0)

Deprecated: count(): Passing null is deprecated in %s on line %d
int(0)

Deprecated: count(): Passing null is deprecated in %s on line %d

Warning: Undefined variable $var in %s on line %d
int(0)

Warning: Undefined variable $var in %s on line %d

Deprecated: count(): Passing null is deprecated in %s on line %d
int(0)

Warning: Undefined variable $var in %s on line %d

Deprecated: count(): Passing null is deprecated in %s on line %d
int(0)

Notice: Undefined index: key in %s on line %d

Deprecated: count(): Passing null is deprecated in %s on line %d
int(0)

Notice: Undefined index: key in %s on line %d

Deprecated: count(): Passing null is deprecated in %s on line %d
int(0)

Notice: Undefined index: key in %s on line %d

Deprecated: count(): Passing null is deprecated in %s on line %d
int(0)
