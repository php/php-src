--TEST--
DCE 009: Foreach over empty array is a no-op
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
class Loop {
    const VALUES = [];
    public static function test() {
        echo "Start\n";
        $y = [];
        foreach ($y as $x) {
        }
        echo "Done\n";
    }
    public static function test2() {
        foreach (self::VALUES as $x) {
        }
    }
    public static function test3() {
        foreach ([] as $k => &$v) {
        }
    }
}
Loop::test();
Loop::test2();
Loop::test3();
--EXPECTF--
$_main:
     ; (lines=7, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %sdce_009.php:1-23
0000 INIT_STATIC_METHOD_CALL 0 string("Loop") string("test")
0001 DO_UCALL
0002 INIT_STATIC_METHOD_CALL 0 string("Loop") string("test2")
0003 DO_UCALL
0004 INIT_STATIC_METHOD_CALL 0 string("Loop") string("test3")
0005 DO_UCALL
0006 RETURN int(1)

Loop::test:
     ; (lines=3, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %sdce_009.php:4-10
0000 ECHO string("Start
")
0001 ECHO string("Done
")
0002 RETURN null

Loop::test2:
     ; (lines=1, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %sdce_009.php:11-14
0000 RETURN null

Loop::test3:
     ; (lines=3, args=0, vars=0, tmps=1)
     ; (after optimizer)
     ; %sdce_009.php:15-18
0000 V0 = FE_RESET_RW array(...) 0001
0001 FE_FREE V0
0002 RETURN null
Start
Done
