--TEST--
GH-8080 (ReflectionClass::getConstants() depends on def. order)
--FILE--
<?php
class A {
    const LIST = [
        self::TEST => 'Test',
    ];
    private const TEST = 'test';
}

class B extends A {}

$r = new ReflectionClass(B::class);
var_dump(
    $r->getConstants(),
    $r->getConstant("LIST")
);
?>
--EXPECT--
array(1) {
  ["LIST"]=>
  array(1) {
    ["test"]=>
    string(4) "Test"
  }
}
array(1) {
  ["test"]=>
  string(4) "Test"
}
