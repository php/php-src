--TEST--
Argument name for Closure::__invoke via call_user_func reference warning
--FILE--
<?php

$test = function(&$arg) {};
call_user_func([$test, '__invoke'], null);

?>
--EXPECTF--
Warning: Closure::__invoke(): Argument #1 ($arg) must be passed by reference, value given in %s on line %d
