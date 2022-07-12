--TEST--
Asymmetric visibility reference
--FILE--
<?php

class Foo {
    public private(set) int $bar = 0;

    public function test() {
        $bar = &$this->bar;
        $bar++;
    }
}

$foo = new Foo();

try {
    $bar = &$foo->bar;
    $bar++;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
var_dump($foo->bar);

$foo->test();
var_dump($foo->bar);

?>
--EXPECT--
Cannot indirectly modify private(set) property Foo::$bar from global scope
int(0)
int(1)
