--TEST--
Named flags parameter for Attribute attribute (incorrect parameter name)
--FILE--
<?php

// TODO: This should error at compile-time.
#[Attribute(not_flags: Attribute::TARGET_CLASS)]
class MyAttribute {
}

#[MyAttribute]
function test() {}

(new ReflectionFunction('test'))->getAttributes()[0]->newInstance();

?>
--EXPECTF--
Fatal error: Uncaught Error: Attribute "MyAttribute" cannot target function (allowed targets: class) in %s:%d
Stack trace:
#0 %s(%d): ReflectionAttribute->newInstance()
#1 {main}
  thrown in %s on line %d
