--TEST--
JMP 001: JMP_SET with constant arg
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.opt_debug_level=0x20000
--EXTENSIONS--
opcache
--FILE--
<?php
function test() {
    $var = null;
    $var = $var ?: test2();
    return $var;
}
?>
--EXPECTF--
$_main:
     ; (lines=1, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %s:1-8
0000 RETURN int(1)

test:
     ; (lines=4, args=0, vars=1, tmps=1)
     ; (after optimizer)
     ; %s:2-6
0000 INIT_FCALL_BY_NAME 0 string("test2")
0001 V1 = DO_FCALL_BY_NAME
0002 CV0($var) = QM_ASSIGN V1
0003 RETURN CV0($var)
