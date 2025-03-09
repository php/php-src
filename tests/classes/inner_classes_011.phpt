--TEST--
::class, statics, and inner classes
--FILE--
<?php

class Outer {
    class Middle {
        public const FOO = 'foo';
        public static int $bar = 42;
        class Inner {
            public const FOO = 'foo';
            public static int $bar = 42;
        }
    }
}

var_dump(Outer:>Middle::class);
var_dump(Outer:>Middle::FOO);
var_dump(Outer:>Middle::$bar);
var_dump(Outer:>Middle:>Inner::class);
var_dump(Outer:>Middle:>Inner::FOO);
var_dump(Outer:>Middle:>Inner::$bar);
?>
--EXPECT--
string(13) "Outer:>Middle"
string(3) "foo"
int(42)
string(20) "Outer:>Middle:>Inner"
string(3) "foo"
int(42)
