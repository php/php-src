--TEST--
Type inference of frameless functions
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.opt_debug_level=0x20000
--EXTENSIONS--
opcache
--FILE--
<?php
function _strpos(string $str): int|false {
    return \strpos($str, 'o', 1);
}
?>
--EXPECTF--
$_main:
     ; (lines=1, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %sinference_frameless.php:1-6
0000 RETURN int(1)

_strpos:
     ; (lines=4, args=1, vars=1, tmps=1)
     ; (after optimizer)
     ; %sinference_frameless.php:2-4
0000 CV0($str) = RECV 1
0001 FRAMELESS_ICALL_3(strpos) CV0($str) string("o")
0002 T1 = OP_DATA int(1)
0003 RETURN T1
