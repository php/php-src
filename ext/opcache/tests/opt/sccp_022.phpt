--TEST--
SCCP 022: Invalid types
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
function foo(int $x) {
    $a[0] = $x;
    $a[1] = 5;
    echo $a[1];
    $a->foo = 5;
    echo $a[1];
}
function bar() {
    foreach ($a as $v) {
        foreach ($v as $v2) {}
    }
}
?>
--EXPECTF--
$_main:
     ; (lines=1, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %s
0000 RETURN int(1)

foo:
     ; (lines=11, args=1, vars=2, tmps=1)
     ; (after optimizer)
     ; %s
0000 CV0($x) = RECV 1
0001 ASSIGN_DIM CV1($a) int(0)
0002 OP_DATA CV0($x)
0003 ASSIGN_DIM CV1($a) int(1)
0004 OP_DATA int(5)
0005 ECHO string("5")
0006 ASSIGN_OBJ CV1($a) string("foo")
0007 OP_DATA int(5)
0008 T2 = FETCH_DIM_R CV1($a) int(1)
0009 ECHO T2
0010 RETURN null

bar:
     ; (lines=9, args=0, vars=3, tmps=2)
     ; (after optimizer)
     ; %s
0000 V3 = FE_RESET_R CV0($a) 0007
0001 FE_FETCH_R V3 CV1($v) 0007
0002 V4 = FE_RESET_R CV1($v) 0005
0003 FE_FETCH_R V4 CV2($v2) 0005
0004 JMP 0003
0005 FE_FREE V4
0006 JMP 0001
0007 FE_FREE V3
0008 RETURN null
LIVE RANGES:
     3: 0001 - 0007 (loop)
     4: 0003 - 0005 (loop)
