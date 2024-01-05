--TEST--
Defaulted type parameters during inheritance
--FILE--
<?php

abstract class WithSimpleDefault<T1, T2 = int> {
    public function method(T1 $param1, T2 $param2) {
        var_dump($param1);
        var_dump($param2);
    }
}

class Concrete extends WithSimpleDefault<string> {
}

abstract class WithPrevParamDefault<T1, T2 = T1> {
    public function method(T1 $param1, T2 $param2) {
        var_dump($param1);
        var_dump($param2);
    }
}

class Concrete2 extends WithPrevParamDefault<string> {
}

$obj = new Concrete;
$obj->method("string", 42);
try {
    $obj->method(42, "string");
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

$obj = new Concrete2;
$obj->method("string", "string");
try {
    $obj->method([], []);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
string(6) "string"
int(42)
WithSimpleDefault::method(): Argument #2 ($param2) must be of type T2 (where T2 = int), string given, called in %s on line %d
string(6) "string"
string(6) "string"
WithPrevParamDefault::method(): Argument #1 ($param1) must be of type T1 (where T1 = string), array given, called in %s on line %d
