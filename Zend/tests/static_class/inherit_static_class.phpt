--TEST--
Tests that a static class can inherit from another static class
--FILE--
<?php

static class C {
    static function F() {
        echo 'OK';
    }
}

static class C2 extends C {}

C2::F();
?>
--EXPECT--
OK
