--TEST--
Zend: Test object_init_with_constructor() API for objects without constructors
--EXTENSIONS--
zend_test
--FILE--
<?php

class TestUserWithoutConstructor {
    public function __destruct() {
        echo 'Destructor for ', __CLASS__, PHP_EOL;
    }
}

echo "#### Passing no args ####\n";
echo "Userland class:\n";
echo "Using new:\n";
$o = new TestUserWithoutConstructor();
var_dump($o);
unset($o);
echo "Using zend_object_init_with_constructor():\n";
$o = zend_object_init_with_constructor("TestUserWithoutConstructor");
var_dump($o);
unset($o);

echo "Internal class:\n";
echo "Using new:\n";
$o = new _ZendTestMagicCall();
var_dump($o);
unset($o);
echo "Using zend_object_init_with_constructor():\n";
$o = zend_object_init_with_constructor("_ZendTestMagicCall");
var_dump($o);
unset($o);

echo "\n#### Passing extra positional args ####\n";
echo "Userland class:\n";
echo "Using new:\n";
$o = new TestUserWithoutConstructor('position_arg');
var_dump($o);
unset($o);
echo "Using zend_object_init_with_constructor():\n";
$o = zend_object_init_with_constructor("TestUserWithoutConstructor", 'position_arg');
var_dump($o);
unset($o);

echo "Internal class:\n";
echo "Using new:\n";
try {
    $o = new _ZendTestMagicCall('position_arg');
    var_dump($o);
    unset($o);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
echo "Using zend_object_init_with_constructor():\n";
try {
    $o = zend_object_init_with_constructor("_ZendTestMagicCall", 'position_arg');
    var_dump($o);
    unset($o);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

echo "\n#### Passing extra named args ####\n";
echo "Userland class:\n";
echo "Using new:\n";
try {
    $o = new TestUserWithoutConstructor(unknown_param: 'named_arg');
    var_dump($o);
    unset($o);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
echo "Using zend_object_init_with_constructor():\n";
try {
    $o = zend_object_init_with_constructor("TestUserWithoutConstructor", unknown_param: 'named_arg');
    var_dump($o);
    unset($o);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

echo "Internal class:\n";
echo "Using new:\n";
try {
    $o = new _ZendTestMagicCall(unknown_param: 'named_arg');
    var_dump($o);
    unset($o);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
echo "Using zend_object_init_with_constructor():\n";
try {
    $o = zend_object_init_with_constructor("_ZendTestMagicCall", unknown_param: 'named_arg');
    var_dump($o);
    unset($o);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
#### Passing no args ####
Userland class:
Using new:
object(TestUserWithoutConstructor)#1 (0) {
}
Destructor for TestUserWithoutConstructor
Using zend_object_init_with_constructor():
object(TestUserWithoutConstructor)#1 (0) {
}
Destructor for TestUserWithoutConstructor
Internal class:
Using new:
object(_ZendTestMagicCall)#1 (0) {
}
Using zend_object_init_with_constructor():
object(_ZendTestMagicCall)#1 (0) {
}

#### Passing extra positional args ####
Userland class:
Using new:
object(TestUserWithoutConstructor)#1 (0) {
}
Destructor for TestUserWithoutConstructor
Using zend_object_init_with_constructor():
object(TestUserWithoutConstructor)#1 (0) {
}
Destructor for TestUserWithoutConstructor
Internal class:
Using new:
object(_ZendTestMagicCall)#1 (0) {
}
Using zend_object_init_with_constructor():
object(_ZendTestMagicCall)#1 (0) {
}

#### Passing extra named args ####
Userland class:
Using new:
Error: Unknown named parameter $unknown_param
Using zend_object_init_with_constructor():
Error: Unknown named parameter $unknown_param
Internal class:
Using new:
Error: Unknown named parameter $unknown_param
Using zend_object_init_with_constructor():
Error: Unknown named parameter $unknown_param
