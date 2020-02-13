--TEST--
Type errors for methods from traits should refer to using class
--FILE--
<?php

trait T {
    public function test1($arg): int {
        return $arg;
    }
    public function test2(int $arg) {
    }
    public function test3(int $arg = 42) {
    }
}

class C {
    use T;
}
class P extends C {
}

$c = new C;
try {
    $c->test1("foo");
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    $c->test2("foo");
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    $c->test3("foo");
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
Return value of C::test1() must be of the type int, string returned
Argument 1 passed to C::test2() must be of the type int, string given, called in %s on line %d
Argument 1 passed to C::test3() must be of the type int, string given, called in %s on line %d
