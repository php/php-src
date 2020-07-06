--TEST--
Attempting to access static properties using instance property syntax
--FILE--
<?php
class C {
    protected static $y = 'C::$y';
}
$c = new C;

echo "\n--> Access non-visible static prop like instance prop:\n";
echo $c->y;
?>
===DONE===
--EXPECTF--
--> Access non-visible static prop like instance prop:

Fatal error: Uncaught Error: Protected property C::$y cannot be accessed from the global scope in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line 8
