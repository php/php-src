--TEST--
Block Pass 007: BOOL + TYPE_CHECK
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.opt_debug_level=0x20000
--EXTENSIONS--
opcache
--FILE--
<?php
$f = random_int(1, 2);

var_dump(!$f === true);
var_dump(!$f === false);
var_dump(!!$f === true);
var_dump(!!$f === false);
?>
--EXPECTF--
$_main:
     ; (lines=22, args=0, vars=1, tmps=%d)
     ; (after optimizer)
     ; %s
0000 INIT_FCALL 2 %d string("random_int")
0001 SEND_VAL int(1) 1
0002 SEND_VAL int(2) 2
0003 V1 = DO_ICALL
0004 ASSIGN CV0($f) V1
0005 INIT_FCALL 1 %d string("var_dump")
0006 T1 = BOOL_NOT CV0($f)
0007 SEND_VAL T1 1
0008 DO_ICALL
0009 INIT_FCALL 1 %d string("var_dump")
0010 T1 = BOOL CV0($f)
0011 SEND_VAL T1 1
0012 DO_ICALL
0013 INIT_FCALL 1 %d string("var_dump")
0014 T1 = BOOL CV0($f)
0015 SEND_VAL T1 1
0016 DO_ICALL
0017 INIT_FCALL 1 %d string("var_dump")
0018 T1 = BOOL_NOT CV0($f)
0019 SEND_VAL T1 1
0020 DO_ICALL
0021 RETURN int(1)
bool(false)
bool(true)
bool(true)
bool(false)
