--TEST--
ReflectionMethod::__construct() tests
--FILE--
<?php

$a = array("", 1, "::", "a::", "::b", "a::b");

foreach ($a as $val) {
    try {
        new ReflectionMethod($val);
    } catch (Exception $e) {
        var_dump($e->getMessage());
    }

    try {
        ReflectionMethod::createFromMethodName($val);
    } catch (Exception $e) {
        var_dump($e->getMessage());
    }
}

$a = array("", 1, "");
$b = array("", "", 1);

foreach ($a as $key=>$val) {
    try {
        new ReflectionMethod($val, $b[$key]);
    } catch (Exception $e) {
        var_dump($e->getMessage());
    }
}

echo "Done\n";
?>
--EXPECT--
string(90) "ReflectionMethod::__construct(): Argument #1 ($objectOrMethod) must be a valid method name"
string(91) "ReflectionMethod::createFromMethodName(): Argument #1 ($method) must be a valid method name"
string(90) "ReflectionMethod::__construct(): Argument #1 ($objectOrMethod) must be a valid method name"
string(91) "ReflectionMethod::createFromMethodName(): Argument #1 ($method) must be a valid method name"
string(23) "Class "" does not exist"
string(23) "Class "" does not exist"
string(24) "Class "a" does not exist"
string(24) "Class "a" does not exist"
string(23) "Class "" does not exist"
string(23) "Class "" does not exist"
string(24) "Class "a" does not exist"
string(24) "Class "a" does not exist"
string(23) "Class "" does not exist"
string(24) "Class "1" does not exist"
string(23) "Class "" does not exist"
Done
