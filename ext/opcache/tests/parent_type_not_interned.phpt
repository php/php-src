--TEST--
Parent type with interning disabled
--INI--
opcache.interned_strings_buffer=0
--FILE--
<?php

class Foo {
    public function test(): self {
    }
}

class Bar extends Foo {
    public function test(): parent {
    }
}

?>
===DONE===
--EXPECT--
===DONE===
