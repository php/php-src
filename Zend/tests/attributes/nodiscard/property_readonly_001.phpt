--TEST--
#[\NoDiscard]: NoDiscard::$message is readonly.
--FILE--
<?php

$d = new \NoDiscard("foo");
$d->message = 'bar';

?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot modify readonly property NoDiscard::$message in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
