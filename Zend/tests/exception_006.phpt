--TEST--
Trying to throw a non-object
--FILE--
<?php

throw 1;

?>
--EXPECTF--
Fatal error: Uncaught Error: Can only throw objects in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
