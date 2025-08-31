--TEST--
GH-17151: ZEND_FETCH_OBJ_R may modify RC of op1
--FILE--
<?php

class C {
    public static $prop;

    public function __get($name) {
        C::$prop = null;
    }

    public function __destruct() {
        echo __METHOD__, "\n";
    }
}

function test() {
    C::$prop = new C();
    C::$prop->bar;
}

test();
echo "Done\n";

?>
--EXPECT--
C::__destruct
Done
