--TEST--
Namespaced type declarations with wrong-cased namespace fails with wrong case
--FILE--
<?php
namespace MyApp;

class Foo {}

function test(\myapp\Foo $x): \myapp\Foo { return $x; }

$obj = new Foo();
$result = test($obj);
echo get_class($result) . "\n";
?>
--EXPECTF--
Fatal error: Uncaught TypeError: MyApp\test(): Argument #1 ($x) must be of type myapp\Foo, MyApp\Foo given, called in %s on line %d and defined in %s:%d
Stack trace:
#0 %s(9): MyApp\test(Object(MyApp\Foo))
#1 {main}
  thrown in %s on line %d
