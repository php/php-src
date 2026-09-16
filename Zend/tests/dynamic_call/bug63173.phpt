--TEST--
Bug #63173: Crash when invoking invalid array callback
--FILE--
<?php

// the important part here are the indexes 1 and 2
$callback = [1 => 0, 2 => 0];
$callback();

?>
--EXPECTF--
Fatal error: Uncaught Error: Array callback has to contain indices 0 and 1 in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
