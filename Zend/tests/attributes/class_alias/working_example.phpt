--TEST--
Working usage
--FILE--
<?php

#[ClassAlias('Other')]
class Demo {}

var_dump( class_exists( 'Other' ) );

$obj = new Other();
var_dump( $obj );
?>
--EXPECTF--
bool(true)
object(Demo)#%d (0) {
}
