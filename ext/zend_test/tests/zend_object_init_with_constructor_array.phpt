--TEST--
Zend: Test object_init_with_constructor() API
--EXTENSIONS--
zend_test
--FILE--
<?php

class TestUserWithConstructorArgs {
    public function __construct(int $int_param, string $string_param) {
    }
    public function __destruct() {
        echo 'Destructor for ', __CLASS__, PHP_EOL;
    }
}

class TestUserWithConstructorNoParams {
    public function __construct() {
    }
    public function __destruct() {
        echo 'Destructor for ', __CLASS__, PHP_EOL;
    }
}

class TestUserWithoutConstructor {
    public function __destruct() {
        echo 'Destructor for ', __CLASS__, PHP_EOL;
    }
}

$args = [5, 'str'];
$argsNamed = [5, 'string_param' => 'str'];
$argsNamedExtraUnknown = [5, 'string_param' => 'str', 'unknown' => 'unknown'];

echo "Testing param passing\n";
echo "Test TestUserWithConstructorArgs:\n";
$o = zend_object_init_with_constructor_array("TestUserWithConstructorArgs", $args);
var_dump($o);
unset($o);

$o = zend_object_init_with_constructor_array("TestUserWithConstructorArgs", $argsNamed);
var_dump($o);
unset($o);

try {
	$o = zend_object_init_with_constructor_array("TestUserWithConstructorArgs", $argsNamedExtraUnknown);
	var_dump($o);
	unset($o);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

echo "Test TestUserWithConstructorNoParams:\n";
try {
    $o = zend_object_init_with_constructor_array("TestUserWithConstructorNoParams", $args);
    var_dump($o);
    unset($o);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
	$o = zend_object_init_with_constructor_array("TestUserWithConstructorNoParams", $argsNamed);
	var_dump($o);
	unset($o);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
	$o = zend_object_init_with_constructor_array("TestUserWithConstructorNoParams", $argsNamedExtraUnknown);
	var_dump($o);
	unset($o);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

echo "Test TestUserWithoutConstructor:\n";
$o = zend_object_init_with_constructor_array("TestUserWithoutConstructor", $args);
var_dump($o);
unset($o);

try {
    $o = zend_object_init_with_constructor_array("TestUserWithoutConstructor", $argsNamed);
    var_dump($o);
    unset($o);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    $o = zend_object_init_with_constructor_array("TestUserWithoutConstructor", $argsNamedExtraUnknown);
    var_dump($o);
    unset($o);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

echo "Test _ZendTestMagicCall (internal class no ctor):\n";
try {
    $o = zend_object_init_with_constructor_array("_ZendTestMagicCall", $args);
    var_dump($o);
    unset($o);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    $o = zend_object_init_with_constructor_array("_ZendTestMagicCall", $argsNamed);
    var_dump($o);
    unset($o);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    $o = zend_object_init_with_constructor_array("_ZendTestMagicCall", $argsNamedExtraUnknown);
    var_dump($o);
    unset($o);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Testing param passing
Test TestUserWithConstructorArgs:
object(TestUserWithConstructorArgs)#1 (0) {
}
Destructor for TestUserWithConstructorArgs
object(TestUserWithConstructorArgs)#1 (0) {
}
Destructor for TestUserWithConstructorArgs
Error: Unknown named parameter $unknown
Test TestUserWithConstructorNoParams:
object(TestUserWithConstructorNoParams)#1 (0) {
}
Destructor for TestUserWithConstructorNoParams
Error: Unknown named parameter $string_param
Error: Unknown named parameter $string_param
Test TestUserWithoutConstructor:
object(TestUserWithoutConstructor)#3 (0) {
}
Destructor for TestUserWithoutConstructor
Error: Unknown named parameter $string_param
Error: Unknown named parameter $string_param
Test _ZendTestMagicCall (internal class no ctor):
object(_ZendTestMagicCall)#2 (0) {
}
Error: Unknown named parameter $string_param
Error: Unknown named parameter $string_param
