--TEST--
#[\NonpublicConstructor]: using the attribute itself as an object
--FILE--
<?php

$attrib = new NonpublicConstructor('example');
var_dump( $attrib );

?>
--EXPECTF--
object(NonpublicConstructor)#%d (1) {
  ["message"]=>
  string(7) "example"
}
