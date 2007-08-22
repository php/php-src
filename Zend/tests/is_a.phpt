--TEST--
is_a() and is_subclass_of() shouldn't call __autoload
--INI--
error_reporting=4095
--FILE--
<?php
function __autoload($name) {
	echo("AUTOLOAD '$name'\n");
	eval("class $name {}");
}

class BASE {
}

interface INT {
}

class A extends BASE implements INT {
}

$a = new A;
var_dump(is_a($a, "B1"));
var_dump(is_a($a, "A"));
var_dump(is_a($a, "BASE"));
var_dump(is_a($a, "INT"));
var_dump(is_subclass_of($a, "B2"));
var_dump(is_subclass_of($a, "A"));
var_dump(is_subclass_of($a, "BASE"));
var_dump(is_subclass_of($a, "INT"));

var_dump(is_subclass_of("X1", "X2"));
?>
--EXPECTF--
Strict Standards: is_a(): Deprecated. Please use the instanceof operator in %sis_a.php on line 17
bool(false)

Strict Standards: is_a(): Deprecated. Please use the instanceof operator in %sis_a.php on line 18
bool(true)

Strict Standards: is_a(): Deprecated. Please use the instanceof operator in %sis_a.php on line 19
bool(true)

Strict Standards: is_a(): Deprecated. Please use the instanceof operator in %sis_a.php on line 20
bool(true)
bool(false)
bool(false)
bool(true)
bool(false)
AUTOLOAD 'X1'
bool(false)
