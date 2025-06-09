--TEST--
#[ClassAlias] - attributes parameter value types not validated when constructing ClassAlias
--FILE--
<?php

class NonAttribute {}

#[ClassAlias('Other', [new NonAttribute()])]
class Demo {}

$ref = new ReflectionClass( 'Demo' );
$attrib = $ref->getAttributes()[0];
var_dump( $attrib );

var_dump( $attrib->newInstance() );

?>
--EXPECTF--
object(ReflectionAttribute)#%d (1) {
  ["name"]=>
  string(10) "ClassAlias"
}
object(ClassAlias)#%d (1) {
  ["alias"]=>
  string(5) "Other"
}
