--TEST--
First Class Callable from Magic
--FILE--
<?php
class Foo {
    public function __call($method, $args) {
        return $method;
    }

    public static function __callStatic($method, $args) {
        return $method;
    }
}

$foo = new Foo;
$bar = $foo->anythingInstance(...);

echo $bar() . PHP_EOL;

$qux = Foo::anythingStatic(...);

echo $qux();
?>
--EXPECT--
anythingInstance
anythingStatic
