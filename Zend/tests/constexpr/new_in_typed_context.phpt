--TEST--
new as typed property/parameter default
--FILE--
<?php

class Test {
    public stdClass $prop = new stdClass;
}
var_dump(new Test);

class Test2 {
    public stdClass $prop = new Test;
}
try {
    var_dump(new Test2);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

function test(stdClass $arg = new stdClass) {
    var_dump($arg);
}
test();

function test2(stdClass $arg = new Test) {
    var_dump($arg);
}
try {
    test2();
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
object(Test)#1 (1) {
  ["prop"]=>
  object(stdClass)#2 (0) {
  }
}
Cannot assign Test to property Test2::$prop of type stdClass
object(stdClass)#1 (0) {
}
test2(): Argument #1 ($arg) must be of type stdClass, Test given, called in %s on line %d
