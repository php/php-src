--TEST--
GH-9432: Dynamic property deprecation warning/readonly error not emitted when modifying through new ArrayObject($targetObj);
--EXTENSIONS--
spl
--FILE--
<?php
class MyClass {}
$c = new MyClass();
$y = new ArrayObject($c);
$y['foo1'] = 'bar';
var_dump($c->foo1);
$c->foo2 = 2;
var_dump($c);

readonly class ReadOnly_ {}
$r = new ReadOnly_();
$z = new ArrayObject($r);
try {
    $z['foo'] = 'bar';
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
var_dump($r);
?>
--EXPECTF--
Deprecated: Creation of dynamic property MyClass::$foo1 is deprecated in %s on line %d
string(3) "bar"

Deprecated: Creation of dynamic property MyClass::$foo2 is deprecated in %s on line %d
object(MyClass)#1 (2) {
  ["foo1"]=>
  string(3) "bar"
  ["foo2"]=>
  int(2)
}
Error: Cannot create dynamic property ReadOnly_::$foo
object(ReadOnly_)#3 (0) {
}
