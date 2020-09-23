--TEST--
strict_types=1 of the attribute use-site is respected
--FILE--
<?php

#[Attribute]
class MyAttribute {
    public function __construct(public int $value) {}
}

#[MyAttribute("42")]
class TestWeak {}

require __DIR__ . '/030_strict_types.inc';

var_dump((new ReflectionClass(TestWeak::class))->getAttributes()[0]->newInstance());
var_dump((new ReflectionClass(TestStrict::class))->getAttributes()[0]->newInstance());

?>
--EXPECTF--
object(MyAttribute)#1 (1) {
  ["value"]=>
  int(42)
}

Fatal error: Uncaught TypeError: MyAttribute::__construct(): Argument #1 ($value) must be of type int, string given in %s:%d
Stack trace:
#0 [internal function]: MyAttribute->__construct('42')
#1 %s(%d): ReflectionAttribute->newInstance()
#2 {main}
  thrown in %s on line %d
