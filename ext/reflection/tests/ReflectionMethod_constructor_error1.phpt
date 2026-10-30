--TEST--
ReflectionMethod constructor errors
--CREDITS--
Robin Fernandes <robinf@php.net>
Steve Seear <stevseea@php.net>
--FILE--
<?php

class TestClass
{
    public function foo() {
    }
}


try {
    echo "\nWrong type of argument (bool):\n";
    $methodInfo = new ReflectionMethod(true);
} catch (Exception $e) {
    print $e->__toString();
}
try {
    echo "\nWrong type of argument (int):\n";
    $methodInfo = new ReflectionMethod(3);
} catch (Exception $e) {
    print $e->__toString();
}
try {
    echo "\nWrong type of argument (bool, string):\n";
    $methodInfo = new ReflectionMethod(true, "foo");
} catch (Exception $e) {
    print $e->__toString();
}
try {
    echo "\nWrong type of argument (string, bool):\n";
    $methodInfo = new ReflectionMethod('TestClass', true);
} catch (Exception $e) {
    print $e->__toString();
}
try {
    echo "\nNo method given:\n";
    $methodInfo = new ReflectionMethod("TestClass");
} catch (Exception $e) {
    print $e->__toString();
}
try {
    echo "\nClass and Method in same string, bad method name:\n";
    $methodInfo = new ReflectionMethod("TestClass::foop::dedoop");
} catch (Exception $e) {
    print $e->__toString();
}
try {
    echo "\nClass and Method in same string, bad class name:\n";
    $methodInfo = new ReflectionMethod("TestCla::foo");
} catch (Exception $e) {
    print $e->__toString();
}
try {
    echo "\nClass and Method in same string (ok):\n";
    $methodInfo = new ReflectionMethod("TestClass::foo");
} catch (Exception $e) {
    print $e->__toString();
}

?>
--EXPECTF--
Wrong type of argument (bool):

Deprecated: Calling ReflectionMethod::__construct() with 1 argument is deprecated, use ReflectionMethod::createFromMethodName() instead in %s on line %d
ReflectionException: ReflectionMethod::__construct(): Argument #1 ($objectOrMethod) must be a valid method name in %s:%d
Stack trace:
#0 %s ReflectionMethod->__construct('1')
#1 {main}
Wrong type of argument (int):

Deprecated: Calling ReflectionMethod::__construct() with 1 argument is deprecated, use ReflectionMethod::createFromMethodName() instead in %s on line %d
ReflectionException: ReflectionMethod::__construct(): Argument #1 ($objectOrMethod) must be a valid method name in %s:%d
Stack trace:
#0 %s ReflectionMethod->__construct('3')
#1 {main}
Wrong type of argument (bool, string):
ReflectionException: Class "1" does not exist in %s:%d
Stack trace:
#0 %s ReflectionMethod->__construct('1', 'foo')
#1 {main}
Wrong type of argument (string, bool):
ReflectionException: Method TestClass::1() does not exist in %s:%d
Stack trace:
#0 %s ReflectionMethod->__construct('TestClass', '1')
#1 {main}
No method given:

Deprecated: Calling ReflectionMethod::__construct() with 1 argument is deprecated, use ReflectionMethod::createFromMethodName() instead in %s on line %d
ReflectionException: ReflectionMethod::__construct(): Argument #1 ($objectOrMethod) must be a valid method name in %s:%d
Stack trace:
#0 %s ReflectionMethod->__construct('TestClass')
#1 {main}
Class and Method in same string, bad method name:

Deprecated: Calling ReflectionMethod::__construct() with 1 argument is deprecated, use ReflectionMethod::createFromMethodName() instead in %s on line %d
ReflectionException: Method TestClass::foop::dedoop() does not exist in %s:%d
Stack trace:
#0 %s ReflectionMethod->__construct('TestClass::foop...')
#1 {main}
Class and Method in same string, bad class name:

Deprecated: Calling ReflectionMethod::__construct() with 1 argument is deprecated, use ReflectionMethod::createFromMethodName() instead in %s on line %d
ReflectionException: Class "TestCla" does not exist in %s:%d
Stack trace:
#0 %s ReflectionMethod->__construct('TestCla::foo')
#1 {main}
Class and Method in same string (ok):

Deprecated: Calling ReflectionMethod::__construct() with 1 argument is deprecated, use ReflectionMethod::createFromMethodName() instead in %s on line %d
