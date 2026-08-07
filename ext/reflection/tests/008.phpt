--TEST--
ReflectionMethod::__construct() tests
--FILE--
<?php

$a = array("", 1, "::", "a::", "::b", "a::b");

foreach ($a as $val) {
    try {
        new ReflectionMethod($val);
    } catch (Exception $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }

    try {
        ReflectionMethod::createFromMethodName($val);
    } catch (Exception $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
}

$a = array("", 1, "");
$b = array("", "", 1);

foreach ($a as $key=>$val) {
    try {
        new ReflectionMethod($val, $b[$key]);
    } catch (Exception $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
}

echo "Done\n";
?>
--EXPECTF--
Deprecated: Calling ReflectionMethod::__construct() with 1 argument is deprecated, use ReflectionMethod::createFromMethodName() instead in %s on line %d
ReflectionException: ReflectionMethod::__construct(): Argument #1 ($objectOrMethod) must be a valid method name
ReflectionException: ReflectionMethod::createFromMethodName(): Argument #1 ($method) must be a valid method name

Deprecated: Calling ReflectionMethod::__construct() with 1 argument is deprecated, use ReflectionMethod::createFromMethodName() instead in %s on line %d
ReflectionException: ReflectionMethod::__construct(): Argument #1 ($objectOrMethod) must be a valid method name
ReflectionException: ReflectionMethod::createFromMethodName(): Argument #1 ($method) must be a valid method name

Deprecated: Calling ReflectionMethod::__construct() with 1 argument is deprecated, use ReflectionMethod::createFromMethodName() instead in %s on line %d
ReflectionException: Class "" does not exist
ReflectionException: Class "" does not exist

Deprecated: Calling ReflectionMethod::__construct() with 1 argument is deprecated, use ReflectionMethod::createFromMethodName() instead in %s on line %d
ReflectionException: Class "a" does not exist
ReflectionException: Class "a" does not exist

Deprecated: Calling ReflectionMethod::__construct() with 1 argument is deprecated, use ReflectionMethod::createFromMethodName() instead in %s on line %d
ReflectionException: Class "" does not exist
ReflectionException: Class "" does not exist

Deprecated: Calling ReflectionMethod::__construct() with 1 argument is deprecated, use ReflectionMethod::createFromMethodName() instead in %s on line %d
ReflectionException: Class "a" does not exist
ReflectionException: Class "a" does not exist
ReflectionException: Class "" does not exist
ReflectionException: Class "1" does not exist
ReflectionException: Class "" does not exist
Done
