--TEST--
Nullable argument with default NULL value 
--FILE--
<?php
function foo(?array $x = null) {
	echo "ok\n";
}

foo([]);
foo(null);
foo(0);
?>
--EXPECTF--
ok
ok

Catchable fatal error: Argument 1 passed to foo() must be of the type array, integer given, called in %s002.php on line 8 and defined in %s002.php on line 2
