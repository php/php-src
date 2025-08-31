--TEST--
FCC in initializer errors for FCC on Constant.
--FILE--
<?php

const Name = 'strrev';

const Closure = (Name)(...);

var_dump(Closure);

?>
--EXPECTF--
Fatal error: Cannot use dynamic function name in constant expression in %s on line %d
