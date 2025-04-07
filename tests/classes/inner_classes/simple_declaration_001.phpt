--TEST--
simple declaration
--FILE--
<?php

class Outer {
    class Middle {
        class Inner {}
    }
}

var_dump(class_exists('Outer\Middle'));
var_dump(class_exists('Outer\Middle\Inner'));
var_dump(class_exists(Outer\Middle::class));
var_dump(class_exists(Outer\Middle\Inner::class));
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
