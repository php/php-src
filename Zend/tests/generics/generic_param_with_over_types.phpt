--TEST--
Combining a generic parameter with other types
--FILE--
<?php

abstract class AbstractTest1<T> {
    public ?T $prop;
    public function method(?T $param) {
        var_dump($param);
    }
}
class ConcreteTest1 extends AbstractTest1<array> {}

$obj = new ConcreteTest1;
$obj->method([]);
$obj->method(null);
try {
    $obj->method("string");
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

echo "\n";

abstract class AbstractTest2<T> {
    public T|int $prop;
    public function method(T|int $param) {
        var_dump($param);
    }
}
class ConcreteTest2 extends AbstractTest2<array> {}

$obj = new ConcreteTest2;
$obj->method([]);
$obj->method(42);
$obj->method("42");

echo "\n";

abstract class AbstractTest3<T> {
    public T|stdClass $prop;
    public function method(T|stdClass $param) {
        var_dump($param);
    }
}
class ConcreteTest3 extends AbstractTest3<array> {}

$obj = new ConcreteTest3;
$obj->method([]);
$obj->method(new stdClass);
try {
    $obj->method("string");
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
array(0) {
}
NULL
AbstractTest1::method(): Argument #1 ($param) must be of type ?T (where T = array), string given, called in %s on line %d

array(0) {
}
int(42)
int(42)

array(0) {
}
object(stdClass)#3 (0) {
}
AbstractTest3::method(): Argument #1 ($param) must be of type T|stdClass (where T = array), string given, called in %s on line %d
