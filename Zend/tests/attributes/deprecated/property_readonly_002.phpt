--TEST--
#[\Deprecated]: Deprecated::$since is readonly.
--FILE--
<?php

$d = new \Deprecated("foo", "1.0");
$d->since = "2.0";

?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot modify readonly property Deprecated::$since in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
