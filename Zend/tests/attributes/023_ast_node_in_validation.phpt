--TEST--
Attribute flags value is validated.
--FILE--
<?php

#[Attribute(Foo::BAR)]
class A1 { }

?>
--EXPECTF--
Fatal error: Class "Foo" not found in %s on line %d
