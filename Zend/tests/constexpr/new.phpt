--TEST--
new in constant expressions
--FILE--
<?php

try {
    eval('const A = new DoesNotExist;');
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

const B = new stdClass;
var_dump(B);

try {
    eval('const C = new stdClass([] + 0);');
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

class Test {
    public function __construct(public $a, public $b) {}
}

try {
    eval('const D = new Test(new stdClass, [] + 0);');
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

const E = new Test(new stdClass, 42);
var_dump(E);

class Test2 {
    public function __construct() {
        echo "Side-effect\n";
        throw new Exception("Failed to construct");
    }
}

try {
    eval('const F = new Test2();');
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Class "DoesNotExist" not found
object(stdClass)#2 (0) {
}
Unsupported operand types: array + int
Unsupported operand types: array + int
object(Test)#4 (2) {
  ["a"]=>
  object(stdClass)#1 (0) {
  }
  ["b"]=>
  int(42)
}
Side-effect
Failed to construct
