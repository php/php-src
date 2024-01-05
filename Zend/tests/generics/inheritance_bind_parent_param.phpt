--TEST--
Bind direct parent parameter during inheritance
--FILE--
<?php

abstract class WithParam<T> {
    public T $prop;

    public function method(T $param) {
        var_dump($param);
    }
}

class ConcreteInt extends WithParam<int> {
}

class ConcreteStdClass extends WithParam<stdClass> {
}

class ConcreteSelf extends WithParam<self> {
}

$obj = new ConcreteInt;
$obj->method(42);
$obj->prop = 42;
var_dump($obj->prop);

try {
    $obj->method("string");
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    $obj->prop = "string";
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

$obj = new ConcreteStdClass;
$obj->method(new stdClass);
//$obj->prop = new stdClass;
try {
    $obj->method("string");
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

/* TODO: This broke -- "self" resolves to WithParam here.
$obj = new ConcreteSelf;
$obj->method($obj);
try {
    $obj->method(new stdClass);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
*/

?>
--EXPECTF--
int(42)
int(42)
WithParam::method(): Argument #1 ($param) must be of type T (where T = int), string given, called in %s on line %d
Cannot assign string to property WithParam::$prop of type T
object(stdClass)#1 (0) {
}
WithParam::method(): Argument #1 ($param) must be of type T (where T = stdClass), string given, called in %s on line %d
