--TEST--
Nullable return value 
--FILE--
<?php
function foo($x) : ?array {
	return $x;
}

foo([]);
echo "ok\n";
foo(null);
echo "ok\n";
foo(0);
?>
--EXPECTF--
ok
ok

Catchable fatal error: Return value of foo() must be of the type array, integer returned in %s003.php on line 3
