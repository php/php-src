--TEST--
SCCP 032: Yield from optimizations
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.opt_debug_level=0x20000
opcache.preload=
zend_test.observer.enabled=0
--EXTENSIONS--
opcache
--FILE--
<?php
function test(): Generator {
    $result = yield from [];
    $a = [];
    yield from $a;
    yield $result;
    $a[] = 3;
    yield from $a;
}
foreach (test() as $x) {
    var_export($x);
    echo "\n";
}
?>
--EXPECTF--
$_main:
     ; (lines=11, args=0, vars=1, tmps=2)
     ; (after optimizer)
     ; %ssccp_032.php:1-15
0000 INIT_FCALL 0 %d string("test")
0001 V2 = DO_UCALL
0002 V1 = FE_RESET_R V2 0009
0003 FE_FETCH_R V1 CV0($x) 0009
0004 INIT_FCALL 1 %d string("var_export")
0005 SEND_VAR CV0($x) 1
0006 DO_ICALL
0007 ECHO string("
")
0008 JMP 0003
0009 FE_FREE V1
0010 RETURN int(1)
LIVE RANGES:
     1: 0003 - 0009 (loop)

test:
     ; (lines=5, args=0, vars=0, tmps=1)
     ; (after optimizer)
     ; %ssccp_032.php:2-9
0000 GENERATOR_CREATE
0001 YIELD null
0002 T0 = YIELD_FROM array(...)
0003 FREE T0
0004 GENERATOR_RETURN null
NULL
3
