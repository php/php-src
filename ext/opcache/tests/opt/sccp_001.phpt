--TEST--
SCCP 001: Constant propagation
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
function foo() {
    $a = 1;
    $b = $a + 2;
    $a += $b;
    return $a;
}
?>
--EXPECTF--
$_main:
     ; (lines=1, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %ssccp_001.php:1-9
0000 RETURN int(1)

foo:
     ; (lines=1, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %ssccp_001.php:2-7
0000 RETURN int(4)
