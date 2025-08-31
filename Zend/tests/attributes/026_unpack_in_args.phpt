--TEST--
Cannot use unpacking in attribute argument list
--FILE--
<?php

#[MyAttribute(...[1, 2, 3])]
class Foo { }

?>
--EXPECTF--
Fatal error: Cannot use unpacking in attribute argument list in %s on line %d
