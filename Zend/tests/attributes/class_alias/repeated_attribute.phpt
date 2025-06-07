--TEST--
Attribute can be repeated
--FILE--
<?php

#[ClassAlias('First')]
#[ClassAlias('Second')]
class Demo {}

var_dump( class_exists( 'First' ) );
var_dump( class_exists( 'Second' ) );

$obj1 = new First();
var_dump( $obj1 );

$obj2 = new Second();
var_dump( $obj2 );
?>
--EXPECTF--
bool(true)
bool(true)
object(Demo)#%d (0) {
}
object(Demo)#%d (0) {
}
