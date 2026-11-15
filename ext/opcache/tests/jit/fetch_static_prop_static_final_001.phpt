--TEST--
JIT: FETCH_STATIC_PROP static:: in a final class
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit_hot_func=1
--FILE--
<?php
final class FinalClass {
    public static $prop = 'FinalClass';

    function get() {
        return static::$prop;
    }
}

class NonFinal {
    public static $prop = 'NonFinal';

    function get() {
        return static::$prop;
    }
}

class NonFinalChild extends NonFinal {
    public static $prop = 'NonFinalChild';
}

foreach ([new FinalClass, new FinalClass, new NonFinal, new NonFinalChild, new NonFinal] as $obj) {
    var_dump($obj->get());
}
?>
--EXPECT--
string(10) "FinalClass"
string(10) "FinalClass"
string(8) "NonFinal"
string(13) "NonFinalChild"
string(8) "NonFinal"
