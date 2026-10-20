--TEST--
Bug #67938: Segfault when extending interface method with variadic
--FILE--
<?php

interface TestInterface {
    public function foo();
    public function bar(array $bar);
}

class Test implements TestInterface {
    public function foo(...$args) {
        echo __METHOD__, "\n";
    }
    public function bar(array $bar, ...$args) {
        echo __METHOD__, "\n";
    }
}

$obj = new Test;
$obj->foo();
$obj->bar([]);
$obj->bar([], 1);

?>
--EXPECT--
Test::foo
Test::bar
Test::bar
