--TEST--
Type alias cannot reference itself
--FILE--
<?php
use type Foo|int as Foo;
?>
--EXPECTF--
Fatal error: Type alias 'Foo' cannot reference itself in %s on line %d
