--TEST--
First Class Callable from Magic
--FILE--
<?php
class Foo {
    public function __call($method, $args) {
        return $method;
    }

    public static function __callStatic($method, $args) {
        return static::class . "::" . $method;
    }
}

class Bar extends Foo {}

$foo = new Foo;
$bar = $foo->anythingInstance(...);

echo $bar(), "\n";

$qux = Foo::anythingStatic(...);
echo $qux(), "\n";

$qux2 = Bar::anythingStatic(...);
echo $qux2(), "\n";

?>
--EXPECT--
anythingInstance
Foo::anythingStatic
Bar::anythingStatic
