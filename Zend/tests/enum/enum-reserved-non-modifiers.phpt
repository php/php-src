--TEST--
enum keyword is reserved_non_modifiers
--FILE--
<?php

namespace enum {
    class Foo {
        public static function bar() {
            return 'enum\Foo::bar()';
        }
    }
}

namespace {
    class Foo {
        const enum = 'enum const';

        public static function enum() {
            return 'enum static method';
        }
    }

    echo \enum\Foo::bar() . "\n";
    echo Foo::enum . "\n";
    echo Foo::enum() . "\n";
}

?>
--EXPECT--
enum\Foo::bar()
enum const
enum static method
