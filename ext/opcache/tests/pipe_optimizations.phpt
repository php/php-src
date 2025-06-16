--TEST--
Pipe operator optimizes away most callables
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.opt_debug_level=0x20000
--EXTENSIONS--
opcache
--FILE--
<?php

function _test1(int $a): int {
    return $a + 1;
}

class Other {
    public function foo(int $a): int {
        return $a * 2;
    }

    public static function bar(int $a): int {
        return $a -1;
    }
}

$o = new Other();

$res1 = 5
    |> _test1(...)
    |> $o->foo(...)
    |> Other::bar(...)
;

var_dump($res1);
?>
--EXPECTF--
$_main:
     ; (lines=18, args=0, vars=2, tmps=%d)
     ; (after optimizer)
     ; %s:1-27
0000 V2 = NEW 0 string("Other")
0001 DO_FCALL
0002 ASSIGN CV0($o) V2
0003 INIT_FCALL 1 %d string("_test1")
0004 SEND_VAL int(5) 1
0005 T2 = DO_UCALL
0006 INIT_METHOD_CALL 1 CV0($o) string("foo")
0007 SEND_VAL_EX T2 1
0008 V3 = DO_FCALL
0009 T2 = QM_ASSIGN V3
0010 INIT_STATIC_METHOD_CALL 1 string("Other") string("bar")
0011 SEND_VAL T2 1
0012 V2 = DO_UCALL
0013 ASSIGN CV1($res1) V2
0014 INIT_FCALL 1 %d string("var_dump")
0015 SEND_VAR CV1($res1) 1
0016 DO_ICALL
0017 RETURN int(1)
LIVE RANGES:
     2: 0001 - 0002 (new)
     2: 0010 - 0011 (tmp/var)

_test1:
     ; (lines=4, args=1, vars=1, tmps=%d)
     ; (after optimizer)
     ; %s:3-5
0000 CV0($a) = RECV 1
0001 T1 = ADD CV0($a) int(1)
0002 VERIFY_RETURN_TYPE T1
0003 RETURN T1

Other::foo:
     ; (lines=4, args=1, vars=1, tmps=%d)
     ; (after optimizer)
     ; %s:8-10
0000 CV0($a) = RECV 1
0001 T1 = ADD CV0($a) CV0($a)
0002 VERIFY_RETURN_TYPE T1
0003 RETURN T1

Other::bar:
     ; (lines=4, args=1, vars=1, tmps=%d)
     ; (after optimizer)
     ; %s:12-14
0000 CV0($a) = RECV 1
0001 T1 = SUB CV0($a) int(1)
0002 VERIFY_RETURN_TYPE T1
0003 RETURN T1
int(11)
