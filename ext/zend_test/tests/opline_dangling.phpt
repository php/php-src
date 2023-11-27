--TEST--
possible segfault in `ZEND_BIND_STATIC`
--DESCRIPTION--
https://github.com/php/php-src/pull/12758
--EXTENSIONS--
zend_test
--INI--
zend_test.observe_opline_in_zendmm=1
--FILE--
<?php

function &ref() {
    static $a = 5;
    return $a;
}

class Foo {
    public static int $i;
    public static string $s = "x";
}

var_dump(Foo::$i = "1");
var_dump(Foo::$s, Foo::$i);
var_dump(ref());

echo 'Done.';
?>
--EXPECT--
int(1)
string(1) "x"
int(1)
int(5)
Done.
