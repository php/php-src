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

Fatal error: Uncaught TypeError: Return value of foo() must be of the type array or null, int returned in %s030.php:3
Stack trace:
#0 %s030.php(10): foo(0)
#1 {main}
  thrown in %s030.php on line 3
