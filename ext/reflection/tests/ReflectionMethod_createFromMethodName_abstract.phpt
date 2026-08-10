--TEST--
ReflectionMethod::createFromMethodName() called on an abstract subclass
--FILE--
<?php

class C {
    public function a() {}
}

abstract class R extends ReflectionMethod {}

try {
    R::createFromMethodName('C::a');
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

var_dump(ReflectionMethod::createFromMethodName('C::a')->name);

?>
--EXPECT--
Error: Cannot instantiate abstract class R
string(1) "a"
