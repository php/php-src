--TEST--
Match expression with no cases
--FILE--
<?php

$x = match (true) {};

?>
--EXPECTF--
Fatal error: Uncaught UnhandledMatchError: Unhandled match value true in %s
Stack trace:
#0 {main}
  thrown in %s on line %d
