--TEST--
GH-17151: Method calls may modify RC of ZEND_INIT_METHOD_CALL op1
--FILE--
<?php

class C {
    public static $prop;

    public function storeThis() {
        self::$prop = $this;
    }
}

function test() {
    $c = new C();
    $c->storeThis();
    $c = null;
}

test();

?>
===DONE===
--EXPECT--
===DONE===
