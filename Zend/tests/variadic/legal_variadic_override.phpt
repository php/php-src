--TEST--
Cases where non-variadic parameters are allowed to be subsumed by a variadic one
--FILE--
<?php

class A {
    public function test1($a, $b) {}
    public function test2(int $a, int $b) {}
    public function test3(int $a, int $b) {}
    public function test4(int $a, string $b) {}
    public function test5(&$a, &$b) {}
}

class B extends A {
    public function test1(...$args) {}
    public function test2(...$args) {}
    public function test3(int ...$args) {}
    public function test4(int|string ...$args) {}
    public function test5(&...$args) {}
}

?>
===DONE==
--EXPECT--
===DONE==
