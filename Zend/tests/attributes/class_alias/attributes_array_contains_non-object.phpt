--TEST--
#[ClassAlias] - attributes parameter validated at compile time (non-object in array)
--FILE--
<?php

#[ClassAlias('Other', [true, new MissingAttribute()])]
class Demo {}

$ref = new ReflectionClass( 'Demo' );
$attrib = $ref->getAttributes()[0];
var_dump( $attrib );

$attrib->newInstance();

?>
--EXPECTF--
Fatal error: Attribute must be declared with `new` in %s on line %d
