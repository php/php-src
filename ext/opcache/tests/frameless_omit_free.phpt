--TEST--
Frameless returning non-RC types can omit free at compile-time
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.opt_debug_level=0x10000
--FILE--
<?php

function test(array $array) {
    class_exists('Foo');
    preg_match('(foo)', 'foo');
    implode(', ', $array);
}

?>
--EXPECTF--
$_main:
     ; (lines=%d, args=0, vars=0, tmps=%d)
     ; (before optimizer)
     ; %s
     ; return  [] RANGE[0..0]
0000 RETURN int(1)

test:
     ; (lines=%d, args=1, vars=1, tmps=%d)
     ; (before optimizer)
     ; %s
     ; return  [] RANGE[0..0]
0000 CV0($array) = RECV 1
0001 T1 = FRAMELESS_ICALL_1(class_exists) string("Foo")
0002 T2 = FRAMELESS_ICALL_2(preg_match) string("(foo)") string("foo")
0003 T3 = FRAMELESS_ICALL_2(implode) string(", ") CV0($array)
0004 FREE T3
0005 RETURN null
