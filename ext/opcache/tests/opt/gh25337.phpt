--TEST--
GH-25337: Include line numbers in all of OPcache's debug dumps of bytecode
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_cache=
opcache.file_cache_only=0
opcache.opt_debug_level=0x404A0041
--FILE--
<?php

function test(int $x): int {
    return $x + 1;
}

var_dump(test(41));

?>
--EXPECTF--
$_main:
     ; (lines=7, args=0, vars=0, tmps=2)
     ; (after pass 1)
     ; %sgh25337.php:1-10
     ; return  [] RANGE[0..0]
L0007 0000 INIT_FCALL 1 %d string("var_dump")
L0007 0001 INIT_FCALL 1 %d string("test")
L0007 0002 SEND_VAL int(41) 1
L0007 0003 T0 = DO_UCALL
L0007 0004 SEND_VAL T0 1
L0007 0005 DO_ICALL
L0010 0006 RETURN int(1)

$_main:
     ; (lines=7, args=0, vars=0, tmps=2)
     ; (after block pass)
     ; %sgh25337.php:1-10
     ; return  [] RANGE[0..0]
BB0:
     ; start exit lines=[0-6]
L0007 0000 INIT_FCALL 1 %d string("var_dump")
L0007 0001 INIT_FCALL 1 %d string("test")
L0007 0002 SEND_VAL int(41) 1
L0007 0003 T0 = DO_UCALL
L0007 0004 SEND_VAL T0 1
L0007 0005 DO_ICALL
L0010 0006 RETURN int(1)

test:
     ; (lines=6, args=1, vars=1, tmps=1)
     ; (after pass 1)
     ; %sgh25337.php:3-5
     ; return  [] RANGE[0..0]
L0003 0000 CV0($x) = RECV 1
L0004 0001 T1 = ADD CV0($x) int(1)
L0004 0002 VERIFY_RETURN_TYPE T1
L0004 0003 RETURN T1
L0005 0004 VERIFY_RETURN_TYPE
L0005 0005 RETURN null

test:
     ; (lines=4, args=1, vars=1, tmps=1)
     ; (after block pass)
     ; %sgh25337.php:3-5
     ; return  [] RANGE[0..0]
BB0:
     ; start exit lines=[0-3]
L0003 0000 CV0($x) = RECV 1
L0004 0001 T1 = ADD CV0($x) int(1)
L0004 0002 VERIFY_RETURN_TYPE T1
L0004 0003 RETURN T1

$_main:
     ; (lines=7, args=0, vars=0, tmps=2, ssa_vars=1, no_loops)
     ; (after dfa pass)
     ; %sgh25337.php:1-10
     ; return  [long] RANGE[1..1]
BB0:
     ; start exit lines=[0-6]
     ; level=0
L0007 0000 INIT_FCALL 1 %d string("var_dump")
L0007 0001 INIT_FCALL 1 %d string("test")
L0007 0002 SEND_VAL int(41) 1
L0007 0003 #0.T0 [long] = DO_UCALL
L0007 0004 SEND_VAL #0.T0 [long] 1
L0007 0005 DO_ICALL
L0010 0006 RETURN int(1)

test:
     ; (lines=4, args=1, vars=1, tmps=1, ssa_vars=4, no_loops)
     ; (after dfa pass)
     ; %sgh25337.php:3-5
     ; return  [long]
     ; #0.CV0($x) NOVAL [undef]
BB0:
     ; start exit lines=[0-3]
     ; level=0
L0003 0000 #1.CV0($x) [long] RANGE[MIN..MAX] = RECV 1
L0004 0001 #2.T1 [long, double] RANGE[%s] = ADD #1.CV0($x) [long] RANGE[MIN..MAX] int(1)
L0004 0002 VERIFY_RETURN_TYPE #2.T1 [long, double] RANGE[%s] -> #3.T1 [long]
L0004 0003 RETURN #3.T1 [long]

$_main:
     ; (lines=7, args=0, vars=0, tmps=2, no_loops)
     ; (after pass 7)
     ; %sgh25337.php:1-10
     ; return  [long] RANGE[1..1]
L0007 0000 INIT_FCALL 1 %d string("var_dump")
L0007 0001 INIT_FCALL 1 %d string("test")
L0007 0002 SEND_VAL int(41) 1
L0007 0003 T0 = DO_UCALL
L0007 0004 SEND_VAL T0 1
L0007 0005 DO_ICALL
L0010 0006 RETURN int(1)

test:
     ; (lines=4, args=1, vars=1, tmps=1, no_loops)
     ; (after pass 7)
     ; %sgh25337.php:3-5
     ; return  [long]
L0003 0000 CV0($x) = RECV 1
L0004 0001 T1 = ADD CV0($x) int(1)
L0004 0002 VERIFY_RETURN_TYPE T1
L0004 0003 RETURN T1

$_main:
     ; (lines=7, args=0, vars=0, tmps=1)
     ; (after optimizer)
     ; %sgh25337.php:1-10
L0007 0000 INIT_FCALL 1 %d string("var_dump")
L0007 0001 INIT_FCALL 1 %d string("test")
L0007 0002 SEND_VAL int(41) 1
L0007 0003 T0 = DO_UCALL
L0007 0004 SEND_VAL T0 1
L0007 0005 DO_ICALL
L0010 0006 RETURN int(1)

test:
     ; (lines=4, args=1, vars=1, tmps=1)
     ; (after optimizer)
     ; %sgh25337.php:3-5
L0003 0000 CV0($x) = RECV 1
L0004 0001 T1 = ADD CV0($x) int(1)
L0004 0002 VERIFY_RETURN_TYPE T1
L0004 0003 RETURN T1
int(42)
