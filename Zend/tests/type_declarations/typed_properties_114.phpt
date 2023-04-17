--TEST--
Typed property type error through ArrayIterator
--FILE--
<?php
class A implements IteratorAggregate {
    function __construct(
        public string $foo = 'bar'
    ) {}

    function getIterator(): Traversable {
        return new ArrayIterator($this);
    }
}

$obj = new A;
foreach ($obj as $k => &$v) {
    try {
        $v = [];
    } catch (Throwable $e) {
        echo $e->getMessage(), "\n";
    }
}
foreach ($obj as $k => &$v) {
    try {
        $v = [];
    } catch (Throwable $e) {
        echo $e->getMessage(), "\n";
    }
}

var_dump($obj);
?>
--EXPECT--
Cannot assign array to reference held by property A::$foo of type string
Cannot assign array to reference held by property A::$foo of type string
object(A)#1 (1) {
  ["foo"]=>
  &string(3) "bar"
}
