--TEST--
__equals: Basic equality operator behaviour
--FILE--
<?php
include("Equatable.inc");

var_dump(new Equatable(1) < new Equatable(0)); // true
var_dump(new Equatable(1) < new Equatable(1)); // false
var_dump(new Equatable(1) < new Equatable(2)); // false

var_dump(new Equatable(1) > new Equatable(0)); // false
var_dump(new Equatable(1) > new Equatable(1)); // false
var_dump(new Equatable(1) > new Equatable(2)); // true

var_dump(new Equatable(1) <= new Equatable(0)); // true
var_dump(new Equatable(1) <= new Equatable(1)); // true
var_dump(new Equatable(1) <= new Equatable(2)); // false

var_dump(new Equatable(1) >= new Equatable(0)); // false
var_dump(new Equatable(1) >= new Equatable(1)); // true
var_dump(new Equatable(1) >= new Equatable(2)); // true

/* These should use __equals */
var_dump(new Equatable(1) == new Equatable(0)); // false
var_dump(new Equatable(1) == new Equatable(1)); // true
var_dump(new Equatable(1) == new Equatable(2)); // false

var_dump(new Equatable(1) != new Equatable(0)); // true
var_dump(new Equatable(1) != new Equatable(1)); // false
var_dump(new Equatable(1) != new Equatable(2)); // true
/****/

var_dump(new Equatable(1) === new Equatable(0)); // false
var_dump(new Equatable(1) === new Equatable(1)); // false
var_dump(new Equatable(1) === new Equatable(2)); // false

var_dump(new Equatable(1) !== new Equatable(0)); // true
var_dump(new Equatable(1) !== new Equatable(1)); // true
var_dump(new Equatable(1) !== new Equatable(2)); // true

var_dump(new Equatable(1) <=> new Equatable(0)); // -1
var_dump(new Equatable(1) <=> new Equatable(1)); //  0
var_dump(new Equatable(1) <=> new Equatable(2)); //  1

?>
--EXPECT--
bool(true)
bool(false)
bool(false)
bool(false)
bool(false)
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
bool(true)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(false)
bool(false)
bool(true)
bool(true)
bool(true)
int(-1)
int(0)
int(1)
