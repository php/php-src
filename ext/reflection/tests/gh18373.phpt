--TEST--
GH-18373: Don't substitute self/parent with anonymous class
--FILE--
<?php

$o = new class() {
    public function test(): self {}
};

echo (new ReflectionClass($o))->getMethod('test')->getReturnType()->getName(), "\n";

?>
--EXPECT--
self
