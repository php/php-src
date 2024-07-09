--TEST--
Tests that a static class can be marked abstract
--FILE--
<?php

abstract static class C {
    abstract static function F();
}

static class C2 extends C {
    static function F() {
        echo 'OK';
    }
}

C2::F();
?>
--EXPECT--
OK
