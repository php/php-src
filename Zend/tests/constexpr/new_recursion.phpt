--TEST--
new recursion in property default value
--FILE--
<?php

class Test {
    public $test = new Test;
}

try {
    new Test;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

class Test2 {
    // Check property name unmangling.
    private $test = new Test2;
}

try {
    new Test2;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

// Check mutual recursion.
class Test3 {
    public $test4 = new Test4;
}
class Test4 {
    public $test3 = new Test3;
}

try {
    new Test3;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    new Test4;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

// Mutual recursion through constructor.
class Test5 {
    public $test6 = new Test6;
}
class Test6 {
    public function __construct() {
        new Test5;
    }
}

try {
    new Test5;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    new Test6;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Trying to recursively instantiate Test while evaluating default value for Test::$test
Trying to recursively instantiate Test2 while evaluating default value for Test2::$test
Trying to recursively instantiate Test3 while evaluating default value for Test3::$test4
Trying to recursively instantiate Test4 while evaluating default value for Test4::$test3
Trying to recursively instantiate Test5 while evaluating default value for Test5::$test6
Trying to recursively instantiate Test5 while evaluating default value for Test5::$test6
