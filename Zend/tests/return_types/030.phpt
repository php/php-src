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

Fatal error: Uncaught TypeError: foo(): Return value must be of type ?array, int returned in %s:%d
Stack trace:
#0 %s030.php(10): foo(0)
#1 {main}
  thrown in %s030.php on line 3
