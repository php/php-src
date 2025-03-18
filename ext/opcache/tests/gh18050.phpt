--TEST--
GH-18050: Frameless calls break IN_ARRAY optimization
--EXTENSIONS--
opcache
--INI--
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.opt_debug_level=0x20000
--FILE--
<?php

function test($v) {
    $ary = ['x', 'y'];
    var_dump(in_array($v, $ary));
    var_dump(in_array($v, $ary, false));
    var_dump(IN_ARRAY($v, $ary, true));

    if (in_array($v, $ary, true)) {
        echo "True\n";
    }
}
test('x');
test('z');

?>
--EXPECTF--
$_main:
     ; (lines=%d, args=%d, vars=%d, tmps=%d)
     ; (after optimizer)
     ; %sgh18050.php:%s
0000 INIT_FCALL 1 %d string("test")
0001 SEND_VAL string("x") 1
0002 DO_UCALL
0003 INIT_FCALL 1 %d string("test")
0004 SEND_VAL string("z") 1
0005 DO_UCALL
0006 RETURN int(1)

test:
     ; (lines=%d, args=%d, vars=%d, tmps=%d)
     ; (after optimizer)
     ; %sgh18050.php:%s
0000 CV0($v) = RECV 1
0001 INIT_FCALL 1 %d string("var_dump")
0002 T1 = IN_ARRAY 0 CV0($v) array(...)
0003 SEND_VAL T1 1
0004 DO_ICALL
0005 INIT_FCALL 1 %d string("var_dump")
0006 T1 = IN_ARRAY 0 CV0($v) array(...)
0007 SEND_VAL T1 1
0008 DO_ICALL
0009 INIT_FCALL 1 %d string("var_dump")
0010 T1 = IN_ARRAY 1 CV0($v) array(...)
0011 SEND_VAL T1 1
0012 DO_ICALL
0013 T1 = IN_ARRAY 1 CV0($v) array(...)
0014 JMPZ T1 0016
0015 ECHO string("True\n")
0016 RETURN null
bool(true)
bool(true)
bool(true)
True
bool(false)
bool(false)
bool(false)
