--TEST--
In operator tests for arrays and traversables
--FILE--
<?php

$foo = "foo";
var_dump($foo in ["foo"]);
var_dump($foo in ["foobar"]);
var_dump($foo in ["php"]);

echo "\n";

var_dump($foo in [["foo"]]);
var_dump([$foo] in [["foo"]]);

echo "\n";

var_dump("foo" in ["foo"]);
var_dump(["foo"] in [["foo"]]);
var_dump(["bar"] in [["foo"]]);
var_dump(null in [null]);

echo "\n";

var_dump(0 in [0]);
var_dump("0" in [0]);
var_dump(0 in ["0"]);
var_dump(0.0 in [0]);
var_dump(0.1 in [0]);

echo "\n";

var_dump([0] in ["foo", 0, [0]]);
var_dump([0] in [[[0]]]);
var_dump([[0]] in [[[0]]]);

echo "\n";

var_dump(1 in (function(){ yield 1; print "must already have stopped..."; })());
var_dump(0 in (function(){ yield 1; yield 2; })());

?>
--EXPECT--
bool(true)
bool(false)
bool(false)

bool(false)
bool(true)

bool(true)
bool(true)
bool(false)
bool(true)

bool(true)
bool(false)
bool(false)
bool(false)
bool(false)

bool(true)
bool(false)
bool(true)

bool(true)
bool(false)
