--TEST--
SCCP: Evaluate array_key_exists with boolean and integral float keys, but not NAN or null
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
        array_key_exists(NAN, [0 => null]),
        array_key_exists(null, ['' => null]),
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
     ; (lines=11, args=0, vars=0, tmps=1)
     ; (after optimizer)
     ; %s:%d-%d
0000 INIT_FCALL 6 %d string("var_dump")
0001 SEND_VAL bool(true) 1
0002 SEND_VAL bool(false) 2
0003 SEND_VAL bool(true) 3
0004 SEND_VAL bool(false) 4
0005 T0 = ARRAY_KEY_EXISTS float(%r[Nn][Aa][Nn]%r) array(...)
0006 SEND_VAL T0 5
0007 T0 = ARRAY_KEY_EXISTS null array(...)
0008 SEND_VAL T0 6
0009 DO_ICALL
0010 RETURN null
