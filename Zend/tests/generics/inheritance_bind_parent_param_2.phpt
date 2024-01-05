--TEST--
Bind multiple parent parameters during inheritance
--FILE--
<?php

abstract class WithParams<T1, T2> {
    public function method(T1 $param1, T2 $param2) {
        var_dump($param1);
        var_dump($param2);
    }
}

abstract class WithSameParam<T> extends WithParams<T, T> {
    public function method2(T $param) {
        var_dump($param);
    }
}

abstract class WithOneFixedParam<T> extends WithParams<int, T> {
    public function method2(T $param) {
        var_dump($param);
    }
}

abstract class WithFirstNullableParam<T> extends WithParams<?T, T> {
}

class ConcreteIntInt extends WithSameParam<int> {
}

class ConcreteIntString extends WithOneFixedParam<string> {
}

class ConcreteNullableIntInt extends WithFirstNullableParam<int> {
}

$obj = new ConcreteIntInt;
$obj->method(42, 42);
$obj->method2(42);

try {
    $obj->method("string", "string");
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    $obj->method2("string");
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

echo "\n";
$obj = new ConcreteIntString;
$obj->method(42, "string");
$obj->method2("string");

try {
    $obj->method("string", 42);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    $obj->method2(42);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

echo "\n";
$obj = new ConcreteNullableIntInt;
$obj->method(null, 42);

?>
--EXPECTF--
int(42)
int(42)
int(42)
WithParams::method(): Argument #1 ($param1) must be of type T1 (where T1 = int), string given, called in %s on line %d
WithSameParam::method2(): Argument #1 ($param) must be of type T (where T = int), string given, called in %s on line %d

int(42)
string(6) "string"
string(6) "string"
WithParams::method(): Argument #1 ($param1) must be of type T1 (where T1 = int), string given, called in %s on line %d
string(2) "42"

NULL
int(42)
