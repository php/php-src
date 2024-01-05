--TEST--
Bind parent parameter passed through to grandparent during inheritance
--FILE--
<?php

abstract class WithParam<T> {
    public function method(T $param) {
        var_dump($param);
    }
}

abstract class WithParam2<T> extends WithParam<T> {
}

class Concrete extends WithParam2<int> {
}

$obj = new Concrete;
$obj->method(42);

try {
    $obj->method("string");
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
int(42)
WithParam::method(): Argument #1 ($param) must be of type T (where T = int), string given, called in %s on line %d
