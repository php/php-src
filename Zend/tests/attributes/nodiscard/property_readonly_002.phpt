--TEST--
#[\NoDiscard]: __construct() respects that properties are readonly.
--FILE--
<?php

$d = new \NoDiscard("foo");
$d->__construct("bar");

?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot modify readonly property NoDiscard::$message in %s:%d
Stack trace:
#0 %s(%d): NoDiscard->__construct('bar')
#1 {main}
  thrown in %s on line %d
