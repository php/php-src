--TEST--
FCC in initializer errors for FCC on variable.
--FILE--
<?php

const Closure = $foo(...);

var_dump(Closure);

?>
--EXPECTF--
Fatal error: Cannot use dynamic function name in constant expression in %s on line %d
