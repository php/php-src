--TEST--
#[\NoDiscard]: Functions with known-used result use DO_[IU]CALL opcodes
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.opt_debug_level=0x20000
--EXTENSIONS--
opcache
--FILE--
<?php

$f = tmpfile();
flock($f, LOCK_SH | LOCK_NB);
(void)flock($f, LOCK_SH | LOCK_NB);
$success = flock($f, LOCK_SH | LOCK_NB);
fclose($f);

#[\NoDiscard]
function test() {
	return new stdClass();
}

test();
(void)test();
$obj = test();

?>
--EXPECTF--
$_main:
     ; (lines=29, args=0, vars=3, tmps=1)
     ; (after optimizer)
     ; %s
0000 INIT_FCALL 0 %d string("tmpfile")
0001 V3 = DO_ICALL
0002 ASSIGN CV0($f) V3
0003 INIT_FCALL 2 %d string("flock")
0004 SEND_VAR CV0($f) 1
0005 SEND_VAL int(5) 2
0006 DO_FCALL_BY_NAME
0007 INIT_FCALL 2 %d string("flock")
0008 SEND_VAR CV0($f) 1
0009 SEND_VAL int(5) 2
0010 V3 = DO_ICALL
0011 FREE V3
0012 INIT_FCALL 2 %d string("flock")
0013 SEND_VAR CV0($f) 1
0014 SEND_VAL int(5) 2
0015 V3 = DO_ICALL
0016 ASSIGN CV1($success) V3
0017 INIT_FCALL 1 %d string("fclose")
0018 SEND_VAR CV0($f) 1
0019 DO_ICALL
0020 INIT_FCALL 0 %d string("test")
0021 DO_FCALL_BY_NAME
0022 INIT_FCALL 0 %d string("test")
0023 V3 = DO_UCALL
0024 FREE V3
0025 INIT_FCALL 0 %d string("test")
0026 V3 = DO_UCALL
0027 ASSIGN CV2($obj) V3
0028 RETURN int(1)

test:
     ; (lines=3, args=0, vars=0, tmps=1)
     ; (after optimizer)
     ; %s
0000 V0 = NEW 0 string("stdClass")
0001 DO_FCALL
0002 RETURN V0
LIVE RANGES:
     0: 0001 - 0002 (new)

Warning: The return value of function flock() should either be used or intentionally ignored by casting it as (void), as locking the stream might have failed in %s on line %d

Warning: The return value of function test() should either be used or intentionally ignored by casting it as (void) in %s on line %d
