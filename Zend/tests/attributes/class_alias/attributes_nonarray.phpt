--TEST--
Alias attributes must be an array
--FILE--
<?php

#[ClassAlias('Other', true)]
class Demo {}

$attr = new ReflectionClass( Demo::class )->getAttributes()[0];
$attr->newInstance();

?>
--EXPECTF--
Fatal error: ClassAlias::__construct(): Argument #2 ($attributes) must be of type array, true given in %s on line %d
