--TEST--
Inline short declaration function
--FILE--
<?php
$f = function() => 123;

var_dump($f());
var_dump((function() => null)());
var_dump(get_debug_type(function() => null()));
var_dump(get_debug_type((function() => null)()));

?>
--EXPECT--
int(123)
NULL
string(7) "Closure"
string(4) "null"
