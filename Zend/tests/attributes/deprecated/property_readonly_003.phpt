--TEST--
#[\Deprecated]: __construct() respects that properties are readonly.
--FILE--
<?php

$d = new \Deprecated("foo");
$d->__construct("bar");

?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot modify readonly property Deprecated::$message in %s:%d
Stack trace:
#0 %s(%d): Deprecated->__construct('bar')
#1 {main}
  thrown in %s on line %d
