--TEST--
Bug #78770: Incorrect callability check inside internal methods
--SKIPIF--
<?php
if (!extension_loaded("intl")) die("skip requires intl");
?>
--FILE--
<?php

class Test {
    public function method() {
        IntlChar::enumCharTypes([$this, 'privateMethod']);
        IntlChar::enumCharTypes('self::privateMethod');
    }

    private function privateMethod($start, $end, $name) {
    }
}

(new Test)->method();

?>
===DONE===
--EXPECT--
===DONE===
