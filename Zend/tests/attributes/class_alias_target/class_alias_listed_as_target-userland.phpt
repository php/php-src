--TEST--
Class alias listed in valid targets when used wrong (userland attribute)
--FILE--
<?php

#[Attribute(Attribute::TARGET_CLASS_ALIAS)]
class MyAliasAttribute {}

#[MyAliasAttribute]
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
    string(16) "MyAliasAttribute"
  }
}

Fatal error: Uncaught Error: Attribute "MyAliasAttribute" cannot target class (allowed targets: class alias) in %s:%d
Stack trace:
#0 %s(%d): ReflectionAttribute->newInstance()
#1 {main}
  thrown in %s on line %d
