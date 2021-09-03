--TEST--
Bug #79193: Incorrect type inference for self::$field =& $field
--EXTENSIONS--
opcache
--FILE--
<?php

class Test {
    public static $foo;
    public static function method($bar) {
        Test::$foo =& $bar;
        var_dump(is_int($bar));
    }
}

Test::method(1);

?>
--EXPECT--
bool(true)
