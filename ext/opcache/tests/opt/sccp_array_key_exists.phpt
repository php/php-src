--TEST--
SCCP: Evaluate array_key_exists with boolean and integral float keys
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
function test() {
    var_dump(
        array_key_exists(true, [1 => null]),
        array_key_exists(false, [1 => null]),
        array_key_exists(-1.0, [-1 => null]),
        array_key_exists(2.0, [1 => null]),
    );
}
?>
--EXPECTF--
$_main:
     ; (lines=1, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %s:%d-%d
0000 RETURN int(1)

test:
     ; (lines=7, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %s:%d-%d
0000 INIT_FCALL 4 %d string("var_dump")
0001 SEND_VAL bool(true) 1
0002 SEND_VAL bool(false) 2
0003 SEND_VAL bool(true) 3
0004 SEND_VAL bool(false) 4
0005 DO_ICALL
0006 RETURN null
