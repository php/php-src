--TEST--
It's not possible to directly instantiate the Generator class
--FILE--
<?php

new Generator;

?>
--EXPECTF--
Fatal error: Uncaught exception 'Error' with message 'The "Generator" class is reserved for internal use and cannot be manually instantiated' in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
