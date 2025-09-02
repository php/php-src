--TEST--
Property constant expression lhs error
--FILE--
<?php

const A_prop = (new A)->prop;

?>
--EXPECTF--
Fatal error: Uncaught Error: Class "A" not found in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
