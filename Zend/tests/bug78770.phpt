--TEST--
Bug #78770: Incorrect callability check inside internal methods
--EXTENSIONS--
intl
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
