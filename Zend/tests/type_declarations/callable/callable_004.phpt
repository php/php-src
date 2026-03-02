--TEST--
callable type#004
--FILE--
<?php

interface Foo {}

class P {
    public function test(Foo|callable $foo) {}
}

class C extends P {
    public function test(Foo|callable $foo) {}
}

?>
===DONE===
--EXPECT--
===DONE===
