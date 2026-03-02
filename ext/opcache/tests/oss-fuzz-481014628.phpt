--TEST--
OSS-Fuzz #481014628: Borked FETCH_W+ZEND_FETCH_GLOBAL_LOCK optimization
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
--FILE--
<?php

function f() {
    return 'foo';
}

function test() {
    global ${f()};
    var_dump($foo);
}

test();
$foo = 42;
test();

?>
--EXPECT--
NULL
int(42)
