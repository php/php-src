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
    public function __destruct() {
        echo 'Destructor for ', __CLASS__, PHP_EOL;
    }
}

class ThrowingUser {
    public function __construct() {
        throw new Exception("Don't construct");
    }
    public function __destruct() {
        echo 'Destructor for ', __CLASS__, PHP_EOL;
    }
}

abstract class AbstractClass {
    public function __construct() {
        return new stdClass();
    }
    public function __destruct() {
        echo 'Destructor for ', __CLASS__, PHP_EOL;
    }
}

class TestUserWithConstructorArgs {
    public function __construct(int $int_param, string $string_param) {
        return new stdClass();
    }
    public function __destruct() {
        echo 'Destructor for ', __CLASS__, PHP_EOL;
    }
}

class TestUserWithConstructorNoParams {
    public function __construct() {
        return new stdClass();
    }
    public function __destruct() {
        echo 'Destructor for ', __CLASS__, PHP_EOL;
    }
}

echo "Testing impossible initializations\n";
try {
    $o = zend_object_init_with_constructor("_ZendTestInterface");
    var_dump($o);
    unset($o);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $o = zend_object_init_with_constructor("_ZendTestTrait");
    var_dump($o);
    unset($o);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $o = zend_object_init_with_constructor("ZendTestUnitEnum");
    var_dump($o);
    unset($o);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $o = zend_object_init_with_constructor("AbstractClass");
    var_dump($o);
    unset($o);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $o = zend_object_init_with_constructor("SysvMessageQueue");
    var_dump($o);
    unset($o);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $o = zend_object_init_with_constructor("PrivateUser");
    var_dump($o);
    unset($o);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $o = zend_object_init_with_constructor("ThrowingUser");
    var_dump($o);
    unset($o);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

echo "Testing param passing\n";
try {
    $o = zend_object_init_with_constructor("TestUserWithConstructorArgs");
    var_dump($o);
    unset($o);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $o = zend_object_init_with_constructor("TestUserWithConstructorArgs", "str", 5);
    var_dump($o);
    unset($o);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $o = zend_object_init_with_constructor("TestUserWithConstructorArgs", 5, string_param: "str", unused_param: 15.3);
    var_dump($o);
    unset($o);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

$o = zend_object_init_with_constructor("TestUserWithConstructorArgs", 5, string_param: "str");
var_dump($o);
unset($o);

echo "Passing too many args to constructor\n";
$o = zend_object_init_with_constructor("TestUserWithConstructorArgs", 5, "str", 'unused_param');
var_dump($o);
unset($o);

echo "Testing class with defined constructor and no params\n";
$o = zend_object_init_with_constructor("TestUserWithConstructorNoParams");
var_dump($o);
unset($o);
?>
--EXPECT--
Testing impossible initializations
Error: Cannot instantiate interface _ZendTestInterface
Error: Cannot instantiate trait _ZendTestTrait
Error: Cannot instantiate enum ZendTestUnitEnum
Error: Cannot instantiate abstract class AbstractClass
Error: Cannot directly construct SysvMessageQueue, use msg_get_queue() instead
Error: Call to private PrivateUser::__construct() from global scope
Exception: Don't construct
Testing param passing
ArgumentCountError: Too few arguments to function TestUserWithConstructorArgs::__construct(), 0 passed and exactly 2 expected
TypeError: TestUserWithConstructorArgs::__construct(): Argument #1 ($int_param) must be of type int, string given
Error: Unknown named parameter $unused_param
object(TestUserWithConstructorArgs)#1 (0) {
}
Destructor for TestUserWithConstructorArgs
Passing too many args to constructor
object(TestUserWithConstructorArgs)#1 (0) {
}
Destructor for TestUserWithConstructorArgs
Testing class with defined constructor and no params
object(TestUserWithConstructorNoParams)#1 (0) {
}
Destructor for TestUserWithConstructorNoParams
