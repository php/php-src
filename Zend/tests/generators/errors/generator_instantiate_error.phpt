--TEST--
It's not possible to directly instantiate the Generator class
--FILE--
<?php

new Generator;

?>
--EXPECTF--
Fatal error: Uncaught Error: Call to private Generator::__construct() from global scope, the "Generator" class is reserved for internal use and cannot be manually instantiated, in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
