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
string(91) "ReflectionMethod::__construct(): Argument #1 ($class_or_method) must be a valid method name"
string(91) "ReflectionMethod::__construct(): Argument #1 ($class_or_method) must be a valid method name"
string(21) "Class  does not exist"
string(22) "Class a does not exist"
string(21) "Class  does not exist"
string(22) "Class a does not exist"
string(21) "Class  does not exist"
string(104) "ReflectionMethod::__construct(): Argument #1 ($class_or_method) must be of type object|string, int given"
string(21) "Class  does not exist"
Done
