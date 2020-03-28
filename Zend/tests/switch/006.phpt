--TEST--
Switch expression with no cases
--FILE--
<?php

$x = true switch {};

?>
--EXPECTF--
Fatal error: Uncaught UnhandledSwitchCaseError in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
