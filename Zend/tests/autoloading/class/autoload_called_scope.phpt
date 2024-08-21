--TEST--
Autoloader should not do anything magic with called scope
--FILE--
<?php

class Test {
    public static function register() {
        autoload_register_class([Test::class, 'autoload']);
    }

    public static function autoload($class) {
        echo "self=" . self::class, ", static=", static::class, "\n";
    }
}

class Test2 extends Test {
    public static function register() {
        autoload_register_class([Test2::class, 'autoload']);
    }

    public static function runTest() {
        class_exists('FooBar');
    }
}

Test::register();
Test2::register();
Test2::runTest();

?>
--EXPECT--
self=Test, static=Test
self=Test, static=Test2
