--TEST--
Trying to throw exception of an interface
--FILE--
<?php

interface a { }

throw new a();

?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot instantiate interface a in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
