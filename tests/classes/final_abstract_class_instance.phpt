--TEST--
You cannot create a final abstract class instance.
--FILE--
<?php

final abstract class Foo
{
}

$foo = new Foo();
?>
--EXPECTF--

Fatal error: Uncaught Error: Cannot instantiate final abstract class Foo in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d