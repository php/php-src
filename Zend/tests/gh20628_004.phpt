--TEST--
Invalid opcode for method call with FETCH_THIS
--FILE--
<?php

class Foo {
    public function bar() {
        var_dump($this, __METHOD__);
    }

    public function test() {
        require __DIR__ . '/gh20628_004.inc';
    }
}

(new Foo)->test();

?>
--EXPECTF--
object(Foo)#%d (0) {
}
string(8) "Foo::bar"
