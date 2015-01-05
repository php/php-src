--TEST--
Attempting to access static properties using instance property syntax 
--FILE--
<?php
class C {
    protected static $y = 'C::$y';
}
$c = new C;

echo "\n--> Access non-visible static prop like instance prop:\n";
$c->y = 1;
?>
==Done==
--EXPECTF--

--> Access non-visible static prop like instance prop:

Fatal error: Cannot access protected property C::$y in %s on line 8
