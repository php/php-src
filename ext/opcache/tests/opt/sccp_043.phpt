--TEST--
SCCP 043: Negative string offsets are evaluated
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.opt_debug_level=0x20000
--EXTENSIONS--
opcache
--FILE--
<?php

function foo1() {
    return "abc"[-1];
}

function foo2() {
    return "abc"[-2];
}

function foo3() {
    return "abc"[-3];
}

function foo4() {
    return "abc"[-4];
}

?>
--EXPECTF--
$_main:
     ; (lines=1, args=0, vars=0, tmps=%d)
     ; (after optimizer)
     ; %s.php:%s
0000 RETURN int(1)

foo1:
     ; (lines=1, args=0, vars=0, tmps=%d)
     ; (after optimizer)
     ; %s.php:%s
0000 RETURN string("c")

foo2:
     ; (lines=1, args=0, vars=0, tmps=%d)
     ; (after optimizer)
     ; %s.php:%s
0000 RETURN string("b")

foo3:
     ; (lines=1, args=0, vars=0, tmps=%d)
     ; (after optimizer)
     ; %s.php:%s
0000 RETURN string("a")

foo4:
     ; (lines=2, args=0, vars=0, tmps=%d)
     ; (after optimizer)
     ; %s.php:%s
0000 T0 = FETCH_DIM_R string("abc") int(-4)
0001 RETURN T0
