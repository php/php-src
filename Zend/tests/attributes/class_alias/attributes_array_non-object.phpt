--TEST--
#[ClassAlias] - attributes parameter validated at compile time (array can be evaluated)
--FILE--
<?php

#[ClassAlias('Other', [true])]
class Demo {}

$ref = new ReflectionClass( 'Demo' );
$attrib = $ref->getAttributes()[0];
var_dump( $attrib );

$attrib->newInstance();

?>
--EXPECTF--
Fatal error: ClassAlias::__construct(): Argument #2 ($attributes) must be an array of objects in %s on line %d
