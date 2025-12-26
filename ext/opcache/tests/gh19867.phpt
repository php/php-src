--TEST--
GH-19867: Avoid capturing nested arrow function parameters
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.opt_debug_level=0x20000
--FILE--
<?php
fn() => fn($a, $b) => $a + $b;
?>
--EXPECTF--
$_main:
     ; (lines=%d, args=0, vars=%d, tmps=%d)
     ; (after optimizer)
     ; %s
0000 T0 = DECLARE_LAMBDA_FUNCTION 0
0001 FREE T0
0002 DEFER_RUN
0003 RETURN int(1)

{closure:%s:%d}:
     ; (lines=%d, args=0, vars=%d, tmps=%d)
     ; (after optimizer)
     ; %s
0000 T0 = DECLARE_LAMBDA_FUNCTION 0
0001 DEFER_RUN
0002 RETURN T0
LIVE RANGES:
     0: 0001 - 0002 (tmp/var)

{closure:%s:%d}:
     ; (lines=%d, args=2, vars=%d, tmps=%d)
     ; (after optimizer)
     ; %s
0000 CV0($a) = RECV 1
0001 CV1($b) = RECV 2
0002 T2 = ADD CV0($a) CV1($b)
0003 DEFER_RUN
0004 RETURN T2
LIVE RANGES:
     2: 0003 - 0004 (tmp/var)
