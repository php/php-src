--TEST--
First Class Callable from Magic
--FILE--
<?php
class Foo {
    public function __call($method, $args) {
        return "OK CALL\n";
    }
    
    public static function __callStatic($method, $args) {
        return "OK CALL STATIC\n";
    }
}

$foo = new Foo;
$bar = $foo->anything(...);

echo $bar();

$qux = Foo::anything(...);

echo $qux();
?>
--EXPECT--
OK CALL
OK CALL STATIC
