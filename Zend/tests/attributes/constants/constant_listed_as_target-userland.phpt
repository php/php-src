--TEST--
Constants listed in valid targets when used wrong (userland attribute)
--FILE--
<?php

#[Attribute(Attribute::TARGET_CONSTANT)]
class MyConstantAttribute {}

#[MyConstantAttribute]
class Example {}

$ref = new ReflectionClass(Example::class);
$attribs = $ref->getAttributes();
var_dump($attribs);
$attribs[0]->newInstance();

?>
--EXPECTF--
array(1) {
  [0]=>
  object(ReflectionAttribute)#%d (1) {
    ["name"]=>
    string(19) "MyConstantAttribute"
  }
}

Fatal error: Uncaught Error: Attribute "MyConstantAttribute" cannot target class (allowed targets: constant) in %s:%d
Stack trace:
#0 %s(%d): ReflectionAttribute->newInstance()
#1 {main}
  thrown in %s on line %d
