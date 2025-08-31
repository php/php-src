--TEST--
Fuction inlining 001: FETCH_THIS is incompatible with unused result
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--FILE--
<?php
class Foo {
    function __construct() {
        Some::foo($this);
    }
}
class Some {
    static function foo() {
    }
}
new Foo;
?>
DONE
--EXPECT--
DONE
