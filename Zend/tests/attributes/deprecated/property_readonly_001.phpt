--TEST--
#[\Deprecated]: Deprecated::$message is readonly.
--FILE--
<?php

$d = new \Deprecated("foo");
$d->message = 'bar';

?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot modify readonly property Deprecated::$message in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
