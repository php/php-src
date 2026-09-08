--TEST--
array_map(): foreach optimization - PFA with non-literal args are not optimizable
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.opt_debug_level=0x20000
--ENV--
A=1
--FILE--
<?php

if (getenv('A')) {
    function plusn($x, $n) {
        return $x + $n;
    }
    function timesn($x, &$n) {
        $result = $x * $n;
        $n = 0;
        return $result;
    }
}

function f($obj) {
    var_dump(array_map(plusn(?, $obj->value), range(1, 2)));
    var_dump(array_map(timesn(?, $obj->value), range(1, 2)));
    var_dump($obj);
}

f((object) ['value' => 1]);

?>
--EXPECTF--
$_main:
     ; (lines=%d, args=0, vars=%d, tmps=%d)
     ; (after optimizer)
     ; %s
0000 INIT_FCALL 1 %d string("getenv")
0001 SEND_VAL string("A") 1
0002 T0 = DO_ICALL
0003 JMPZ T0 0006
0004 DECLARE_FUNCTION string("plusn") 0
0005 DECLARE_FUNCTION string("timesn") 1
0006 INIT_FCALL 1 %d string("f")
0007 T0 = CAST (object) array(...)
0008 SEND_VAL T0 1
0009 DO_UCALL
0010 RETURN int(1)

plusn:
     ; (lines=4, args=2, vars=2, tmps=%d)
     ; (after optimizer)
     ; %s
0000 CV0($x) = RECV 1
0001 CV1($n) = RECV 2
0002 T2 = ADD CV0($x) CV1($n)
0003 RETURN T2

timesn:
     ; (lines=5, args=2, vars=3, tmps=%d)
     ; (after optimizer)
     ; %s.php:%s
0000 CV0($x) = RECV 1
0001 CV1($n) = RECV 2
0002 CV2($result) = MUL CV0($x) CV1($n)
0003 ASSIGN CV1($n) int(0)
0004 RETURN CV2($result)

f:
     ; (lines=39, args=1, vars=1, tmps=%d)
     ; (after optimizer)
     ; %s.php:%s
0000 CV0($obj) = RECV 1
0001 INIT_FCALL 1 %d string("var_dump")
0002 INIT_FCALL 2 %d string("array_map")
0003 INIT_FCALL_BY_NAME 2 string("plusn")
0004 SEND_PLACEHOLDER 1
0005 CHECK_FUNC_ARG 2
0006 V1 = FETCH_OBJ_FUNC_ARG (ref) CV0($obj) string("value")
0007 SEND_FUNC_ARG V1 2
0008 T1 = CALLABLE_CONVERT_PARTIAL %d string("{closure:pfa:f():15}")
0009 SEND_VAL T1 1
0010 INIT_FCALL 2 %d string("range")
0011 SEND_VAL int(1) 1
0012 SEND_VAL int(2) 2
0013 T1 = DO_ICALL
0014 SEND_VAL T1 2
0015 T1 = DO_ICALL
0016 SEND_VAL T1 1
0017 DO_ICALL
0018 INIT_FCALL 1 %d string("var_dump")
0019 INIT_FCALL 2 %d string("array_map")
0020 INIT_FCALL_BY_NAME 2 string("timesn")
0021 SEND_PLACEHOLDER 1
0022 CHECK_FUNC_ARG 2
0023 V1 = FETCH_OBJ_FUNC_ARG (ref) CV0($obj) string("value")
0024 SEND_FUNC_ARG V1 2
0025 T1 = CALLABLE_CONVERT_PARTIAL %d string("{closure:pfa:f():16}")
0026 SEND_VAL T1 1
0027 INIT_FCALL 2 %d string("range")
0028 SEND_VAL int(1) 1
0029 SEND_VAL int(2) 2
0030 T1 = DO_ICALL
0031 SEND_VAL T1 2
0032 T1 = DO_ICALL
0033 SEND_VAL T1 1
0034 DO_ICALL
0035 INIT_FCALL 1 %d string("var_dump")
0036 SEND_VAR CV0($obj) 1
0037 DO_ICALL
0038 RETURN null

$_main:
     ; (lines=4, args=0, vars=1, tmps=%d)
     ; (after optimizer)
     ; %s.php:%s
0000 T1 = DECLARE_LAMBDA_FUNCTION 4294967295 0
0001 BIND_LEXICAL T1 CV0($n)
0002 FREE T1
0003 RETURN int(1)
LIVE RANGES:
     1: 0001 - 0002 (tmp/var)

{closure:pfa:f():15}:
     ; (lines=7, args=1, vars=2, tmps=%d)
     ; (after optimizer)
     ; %s.php:%s
0000 CV0($x) = RECV 1
0001 BIND_STATIC CV1($n)
0002 INIT_FCALL 2 %d string("plusn")
0003 SEND_VAR CV0($x) 1
0004 SEND_VAR CV1($n) 2
0005 T2 = DO_UCALL
0006 RETURN T2
array(2) {
  [0]=>
  int(2)
  [1]=>
  int(3)
}

$_main:
     ; (lines=4, args=0, vars=1, tmps=%d)
     ; (after optimizer)
     ; %s.php:%s
0000 T1 = DECLARE_LAMBDA_FUNCTION 4294967295 0
0001 BIND_LEXICAL (ref) T1 CV0($n)
0002 FREE T1
0003 RETURN int(1)
LIVE RANGES:
     1: 0001 - 0002 (tmp/var)

{closure:pfa:f():16}:
     ; (lines=7, args=1, vars=2, tmps=%d)
     ; (after optimizer)
     ; %s.php:%s
0000 CV0($x) = RECV 1
0001 BIND_STATIC (ref) CV1($n)
0002 INIT_FCALL 2 %d string("timesn")
0003 SEND_VAR CV0($x) 1
0004 SEND_REF CV1($n) 2
0005 T2 = DO_UCALL
0006 RETURN T2
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(0)
}
object(stdClass)#%d (1) {
  ["value"]=>
  int(0)
}
