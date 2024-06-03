--TEST--
Zend: Test object_init_with_constructor() API
--EXTENSIONS--
zend_test
sysvmsg
--FILE--
<?php

class PrivateUser {
    private function __construct() {
        return new stdClass();
    }
}

class ThrowingUser {
    public function __construct() {
        throw new Exception("Don't construct");
    }
}

class TestUser {
    public function __construct(int $int_param, string $string_param) {
        return new stdClass();
    }
}

echo "Testing impossible initializations\n";
try {
    $o = zend_object_init_with_constructor("_ZendTestInterface");
    var_dump($o);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $o = zend_object_init_with_constructor("_ZendTestTrait");
    var_dump($o);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $o = zend_object_init_with_constructor("ZendTestUnitEnum");
    var_dump($o);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $o = zend_object_init_with_constructor("SysvMessageQueue");
    var_dump($o);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $o = zend_object_init_with_constructor("PrivateUser");
    var_dump($o);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $o = zend_object_init_with_constructor("ThrowingUser");
    var_dump($o);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

echo "Testing param passing\n";
try {
    $o = zend_object_init_with_constructor("TestUser");
    var_dump($o);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $o = zend_object_init_with_constructor("TestUser", "str", 5);
    var_dump($o);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $o = zend_object_init_with_constructor("TestUser", 5, string_param: "str");
    var_dump($o);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Testing impossible initializations
Error: Cannot instantiate interface _ZendTestInterface
Error: Cannot instantiate trait _ZendTestTrait
Error: Cannot instantiate enum ZendTestUnitEnum
Error: Cannot directly construct SysvMessageQueue, use msg_get_queue() instead
Error: Call to private PrivateUser::__construct() from global scope
Exception: Don't construct
Testing param passing
ArgumentCountError: Too few arguments to function TestUser::__construct(), 0 passed and exactly 2 expected
TypeError: TestUser::__construct(): Argument #1 ($int_param) must be of type int, string given
object(TestUser)#3 (0) {
}
