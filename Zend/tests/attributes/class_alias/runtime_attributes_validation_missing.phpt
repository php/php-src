--TEST--
#[ClassAlias] - attributes parameter validated at runtime (missing class)
--FILE--
<?php

#[ClassAlias('Other', [new MissingAttribute()])]
class Demo {}

$ref = new ReflectionClass( 'Demo' );
$attrib = $ref->getAttributes()[0];
var_dump( $attrib );

$attrib->newInstance();

?>
--EXPECTF--
object(ReflectionAttribute)#%d (1) {
  ["name"]=>
  string(10) "ClassAlias"
}

Fatal error: Uncaught Error: Class "MissingAttribute" not found in %s:%d
Stack trace:
#0 %s(%d): ReflectionAttribute->newInstance()
#1 {main}
  thrown in %s on line %d
