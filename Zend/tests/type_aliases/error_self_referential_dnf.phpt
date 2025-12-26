--TEST--
Type alias cannot reference itself in DNF type
--FILE--
<?php
use type Bar|(Foo&Baz) as Foo;
?>
--EXPECTF--
Fatal error: Type alias 'Foo' cannot reference itself in %s on line %d
