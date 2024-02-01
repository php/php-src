--TEST--
DCE 005: Elimination of assignment to non-escaping objects (can't remove NEW yet)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.opt_debug_level=0x20000
opcache.preload=
--EXTENSIONS--
opcache
--FILE--
<?php
#[AllowDynamicProperties]
class A {
}
function foo(int $x) {
    $a = new A;
    $a->foo = $x;
}
?>
--EXPECTF--
$_main:
     ; (lines=1, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %s
0000 RETURN int(1)

foo:
     ; (lines=2, args=1, vars=1, tmps=0)
     ; (after optimizer)
     ; %s
0000 CV0($x) = RECV 1
0001 RETURN null
