--TEST--
GH-11245: In some specific cases SWITCH with one default statement will cause segfault (TMP variation)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=0x7FFFBFFF
opcache.opt_debug_level=0x20000
opcache.preload=
--EXTENSIONS--
opcache
--FILE--
<?php
class X {
    // Chosen to test for a memory leak.
    static $prop = "aa";
}
switch (++X::$prop) {
    default:
        if (empty($xx)) {return;}
}
?>
--EXPECTF--
$_main:
     ; (lines=7, args=0, vars=1, tmps=2)
     ; (after optimizer)
     ; %s
0000 T1 = PRE_INC_STATIC_PROP string("prop") string("X")
0001 T2 = ISSET_ISEMPTY_CV (empty) CV0($xx)
0002 JMPZ T2 0005
0003 FREE T1
0004 RETURN null
0005 FREE T1
0006 RETURN int(1)
LIVE RANGES:
     1: 0001 - 0005 (tmp/var)
