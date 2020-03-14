--TEST--
Defining constant twice with two different forms
--FILE--
<?php

define('a', 2);
const a = 1;

?>
--EXPECTF--
Fatal error: Uncaught ValueError: Constant a already defined in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
