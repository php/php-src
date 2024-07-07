--TEST--
Tests that a static class can inherit static members from a trait
--FILE--
<?php

trait T {
    static $P = 'OK';

    static function F() {
        echo self::$P;
    }
}

static class C {
    use T;
}

C::F();
?>
--EXPECT--
OK
