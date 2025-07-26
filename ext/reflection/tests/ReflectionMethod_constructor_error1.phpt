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
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    echo "\nWrong type of argument (int):\n";
    $methodInfo = new ReflectionMethod(3);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    echo "\nWrong type of argument (bool, string):\n";
    $methodInfo = new ReflectionMethod(true, "foo");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    echo "\nWrong type of argument (string, bool):\n";
    $methodInfo = new ReflectionMethod('TestClass', true);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    echo "\nNo method given:\n";
    $methodInfo = new ReflectionMethod("TestClass");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    echo "\nClass and Method in same string, bad method name:\n";
    $methodInfo = new ReflectionMethod("TestClass::foop::dedoop");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    echo "\nClass and Method in same string, bad class name:\n";
    $methodInfo = new ReflectionMethod("TestCla::foo");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    echo "\nClass and Method in same string (ok):\n";
    $methodInfo = new ReflectionMethod("TestClass::foo");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECTF--
Wrong type of argument (bool):

Deprecated: Calling ReflectionMethod::__construct() with 1 argument is deprecated, use ReflectionMethod::createFromMethodName() instead in %s on line %d

Deprecated: ReflectionMethod::__construct(): Passing bool to parameter #1 ($objectOrMethod) of type object|string is deprecated in %s on line %d
ReflectionException: ReflectionMethod::__construct(): Argument #1 ($objectOrMethod) must be a valid method name

Wrong type of argument (int):

Deprecated: Calling ReflectionMethod::__construct() with 1 argument is deprecated, use ReflectionMethod::createFromMethodName() instead in %s on line %d
ReflectionException: ReflectionMethod::__construct(): Argument #1 ($objectOrMethod) must be a valid method name

Wrong type of argument (bool, string):

Deprecated: ReflectionMethod::__construct(): Passing bool to parameter #1 ($objectOrMethod) of type object|string is deprecated in %s on line %d
ReflectionException: Class "1" does not exist

Wrong type of argument (string, bool):

Deprecated: ReflectionMethod::__construct(): Passing bool to parameter #2 ($method) of type ?string is deprecated in %s on line %d
ReflectionException: Method TestClass::1() does not exist

No method given:

Deprecated: Calling ReflectionMethod::__construct() with 1 argument is deprecated, use ReflectionMethod::createFromMethodName() instead in %s on line %d
ReflectionException: ReflectionMethod::__construct(): Argument #1 ($objectOrMethod) must be a valid method name

Class and Method in same string, bad method name:

Deprecated: Calling ReflectionMethod::__construct() with 1 argument is deprecated, use ReflectionMethod::createFromMethodName() instead in %s on line %d
ReflectionException: Method TestClass::foop::dedoop() does not exist

Class and Method in same string, bad class name:

Deprecated: Calling ReflectionMethod::__construct() with 1 argument is deprecated, use ReflectionMethod::createFromMethodName() instead in %s on line %d
ReflectionException: Class "TestCla" does not exist

Class and Method in same string (ok):

Deprecated: Calling ReflectionMethod::__construct() with 1 argument is deprecated, use ReflectionMethod::createFromMethodName() instead in %s on line %d
