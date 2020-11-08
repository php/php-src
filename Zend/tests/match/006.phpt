--TEST--
Match expression with no cases
--FILE--
<?php

$x = match (true) {};

?>
--EXPECTF--
Fatal error: Uncaught UnhandledMatchError: Unhandled match value of type bool in %s
Stack trace:
#0 {main}
  thrown in %s on line %d
