--TEST--
returning private inner from inner method
--FILE--
<?php

class Outer {
    private class PrivateInner {
        public function test() {
            return new PrivateInner();
        }
    }

    public function test(): mixed { return new PrivateInner()->test(); }
}

$foo = new Outer()->test();
var_dump($foo);
?>
--EXPECT--
object(Outer\PrivateInner)#3 (0) {
}
