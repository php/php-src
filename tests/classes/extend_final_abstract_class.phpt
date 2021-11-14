--TEST--
You cannot extend a final abstract class.
--FILE--
<?php

final abstract class Foo
{
}

class Bar extends Foo {}
?>
--EXPECTF--
Fatal error: Class Bar cannot extend final abstract class Foo in %s on line %d
