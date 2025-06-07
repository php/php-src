--TEST--
Parameter must be a string
--FILE--
<?php

#[ClassAlias([])]
class Demo {}

?>
--EXPECTF--
Fatal error: ClassAlias::__construct(): Argument #1 ($alias) must be of type string, array given in %s on line %d
