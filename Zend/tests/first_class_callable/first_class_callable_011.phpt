--TEST--
First Class Callable Attribute Error
--FILE--
<?php
#[Attribute(...)]
class Foo {

}
?>
--EXPECTF--
Fatal error: Cannot create Closure as attribute argument in %s on line 3
