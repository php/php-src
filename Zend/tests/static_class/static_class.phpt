--TEST--
Tests that a class can be marked static and functions as a static class
--FILE--
<?php

static class C {
    static $P = 'OK';

    static function F() {
        echo self::$P;
    }
}

C::F();
?>
--EXPECT--
OK
