--TEST--
Live range of ZEND_NEW must be assigned to correct variable
--FILE--
<?php

class Foo {
    public static function test() {
        self::$property = new self;
    }
}

try {
    Foo::test();
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Access to undeclared static property Foo::$property
