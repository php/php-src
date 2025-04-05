--TEST--
no outer class
--FILE--
<?php

new Outer\Inner();
?>
--EXPECTF--
Fatal error: Uncaught Error: Class "Outer\Inner" not found in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
