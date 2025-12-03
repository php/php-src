--TEST--
Type alias cannot reference itself in union type
--FILE--
<?php
use type int|string|MyType as MyType;
?>
--EXPECTF--
Fatal error: Type alias 'MyType' cannot reference itself in %s on line %d
