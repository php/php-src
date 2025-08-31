--TEST--
Match expression with no cases
--FILE--
<?php

$x = match (true) {};

?>
--EXPECTF--
Fatal error: Uncaught UnhandledMatchError: Unhandled match case true in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
