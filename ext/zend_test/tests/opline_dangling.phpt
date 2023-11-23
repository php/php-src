--TEST--
possible segfault in `ZEND_BIND_STATIC`
--DESCRIPTION--
https://github.com/php/php-src/pull/12758
--EXTENSIONS--
zend_test
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

zend_test_observe_opline_in_zendmm();

var_dump(Foo::$i = "1");
var_dump(Foo::$s, Foo::$i);
var_dump(ref());

zend_test_unobserve_opline_in_zendmm();

echo 'Done.';
?>
--EXPECT--
int(1)
string(1) "x"
int(1)
int(5)
Done.
