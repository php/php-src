--TEST--
Attempting to access static properties using instance property syntax
--FILE--
<?php
class C {
    protected static $y = 'C::$y';
}
$c = new C;

echo "\n--> Access non-visible static prop like instance prop:\n";
try {
    $c->y =& $ref;
} catch (Error $e) {
    echo $e, "\n";
}
?>
==Done==
--EXPECTF--
--> Access non-visible static prop like instance prop:
Error: Cannot access protected property C::$y in %s:9
Stack trace:
#0 {main}

Next Error: Cannot access protected property C::$y in %s:9
Stack trace:
#0 {main}
==Done==
