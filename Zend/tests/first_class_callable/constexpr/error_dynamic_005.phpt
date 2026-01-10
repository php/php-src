--TEST--
FCC in initializer errors for FCC on integer expression
--FILE--
<?php

const Closure = (0)(...);

var_dump(Closure);

?>
--EXPECTF--
Fatal error: Illegal function name in %s on line %d
