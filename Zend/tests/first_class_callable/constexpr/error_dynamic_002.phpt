--TEST--
FCC in initializer errors for FCC on Closure.
--FILE--
<?php

const Closure = (static function () { })(...);

var_dump(Closure);

?>
--EXPECTF--
Fatal error: Cannot use dynamic function name in constant expression in %s on line %d
