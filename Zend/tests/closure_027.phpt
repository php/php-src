--TEST--
Closure 027: Testing Closure type-hint
--FILE--
<?php

function test(closure $a) {
	var_dump($a());
}


test(function() { return new stdclass; });

test(function() { });

$a = function($x) use ($y) {};
test($a);

test(new stdclass);

?>
--EXPECTF--
object(stdClass)#%d (0) {
}
NULL

Notice: Undefined variable: y in %s on line %d

Warning: Missing argument 1 for {closure}(), called in %s on line %d and defined in %s on line %d
NULL

Fatal error: Argument 1 passed to test() must be an instance of Closure, instance of stdClass given, called in %s on line %d and defined in %s on line %d
