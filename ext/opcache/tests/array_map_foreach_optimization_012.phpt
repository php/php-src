--TEST--
array_map(): foreach optimization - function argument is evaluated once
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.opt_debug_level=0x20000
--FILE--
<?php

function plusn($x, $n) {
    return $x + $n;
}

function get_function() {
    var_dump(__FUNCTION__);
    return new class {
        function __invoke($value) {
            return $value + 1;
        }
    };
}

class C {
    function __construct() {
        var_dump(__METHOD__);
    }
    static function f($value) {
        return $value + 1;
    }
}

$array = range(1, 2);

var_dump(array_map(get_function()(...), $array));

var_dump(array_map((new C)::f(...), $array));

$f = function ($value) use (&$f) {
    $f = 'dechex';
    return $value + 1;
};

var_dump(array_map($f(...), $array));

?>
--EXPECTF--
$_main:
     ; (lines=%d, args=0, vars=%d, tmps=%d)
     ; (after optimizer)
     ; %s
0000 INIT_FCALL 2 %d string("range")
0001 SEND_VAL int(1) 1
0002 SEND_VAL int(2) 2
0003 T2 = DO_ICALL
0004 ASSIGN CV0($array) T2
0005 INIT_FCALL 1 %d string("var_dump")
0006 INIT_FCALL 0 %d string("get_function")
0007 T3 = DO_UCALL
0008 TYPE_ASSERT 131079 string("array_map") CV0($array)
0009 T2 = INIT_ARRAY 0 (packed) NEXT
0010 V4 = FE_RESET_R CV0($array) 0018
0011 T8 = FE_FETCH_R V4 T7 0018
0012 T9 = COPY_TMP T3
0013 INIT_DYNAMIC_CALL 1 T9
0014 SEND_VAL_EX T7 1
0015 T7 = DO_FCALL
0016 T2 = ADD_ARRAY_ELEMENT T7 T8
0017 JMP 0011
0018 FE_FREE V4
0019 FREE T3
0020 SEND_VAL T2 1
0021 DO_ICALL
0022 INIT_FCALL 1 %d string("var_dump")
0023 T3 = NEW 0 string("C")
0024 DO_FCALL
0025 TYPE_ASSERT 131079 string("array_map") CV0($array)
0026 T2 = INIT_ARRAY 0 (packed) NEXT
0027 V4 = FE_RESET_R CV0($array) 0036
0028 T7 = FE_FETCH_R V4 T6 0036
0029 T9 = COPY_TMP T3
0030 V8 = FETCH_CLASS (exception) T9
0031 INIT_STATIC_METHOD_CALL 1 V8 string("f")
0032 SEND_VAL_EX T6 1
0033 T6 = DO_FCALL
0034 T2 = ADD_ARRAY_ELEMENT T6 T7
0035 JMP 0028
0036 FE_FREE V4
0037 FREE T3
0038 SEND_VAL T2 1
0039 DO_ICALL
0040 T2 = DECLARE_LAMBDA_FUNCTION 4294967295 0
0041 BIND_LEXICAL (ref) T2 CV1($f)
0042 ASSIGN CV1($f) T2
0043 INIT_FCALL 1 %d string("var_dump")
0044 T3 = QM_ASSIGN CV1($f)
0045 TYPE_ASSERT 131079 string("array_map") CV0($array)
0046 T2 = INIT_ARRAY 0 (packed) NEXT
0047 V4 = FE_RESET_R CV0($array) 0055
0048 T6 = FE_FETCH_R V4 T5 0055
0049 T7 = COPY_TMP T3
0050 INIT_DYNAMIC_CALL 1 T7
0051 SEND_VAL_EX T5 1
0052 T5 = DO_FCALL
0053 T2 = ADD_ARRAY_ELEMENT T5 T6
0054 JMP 0048
0055 FE_FREE V4
0056 FREE T3
0057 SEND_VAL T2 1
0058 DO_ICALL
0059 RETURN int(1)
LIVE RANGES:
     3: 0008 - 0019 (tmp/var)
     2: 0010 - 0020 (tmp/var)
     4: 0011 - 0018 (loop)
     7: 0012 - 0014 (tmp/var)
     8: 0012 - 0016 (tmp/var)
     3: 0024 - 0025 (new)
     3: 0025 - 0037 (tmp/var)
     2: 0027 - 0038 (tmp/var)
     4: 0028 - 0036 (loop)
     6: 0029 - 0032 (tmp/var)
     7: 0029 - 0034 (tmp/var)
     2: 0041 - 0042 (tmp/var)
     3: 0045 - 0056 (tmp/var)
     2: 0047 - 0057 (tmp/var)
     4: 0048 - 0055 (loop)
     5: 0049 - 0051 (tmp/var)
     6: 0049 - 0053 (tmp/var)

{closure:%s:%d}:
     ; (lines=5, args=1, vars=2, tmps=%d)
     ; (after optimizer)
     ; %s.php:%s
0000 CV0($value) = RECV 1
0001 BIND_STATIC (ref) CV1($f)
0002 ASSIGN CV1($f) string("dechex")
0003 T2 = ADD CV0($value) int(1)
0004 RETURN T2

plusn:
     ; (lines=4, args=2, vars=2, tmps=%d)
     ; (after optimizer)
     ; %s
0000 CV0($x) = RECV 1
0001 CV1($n) = RECV 2
0002 T2 = ADD CV0($x) CV1($n)
0003 RETURN T2

get_function:
     ; (lines=7, args=0, vars=0, tmps=%d)
     ; (after optimizer)
     ; %s.php:%s
0000 INIT_FCALL 1 %d string("var_dump")
0001 SEND_VAL string("get_function") 1
0002 DO_ICALL
0003 V1 = DECLARE_ANON_CLASS string("class@anonymous%s")
0004 T0 = NEW 0 V1
0005 DO_FCALL
0006 RETURN T0
LIVE RANGES:
     0: 0005 - 0006 (new)

class@anonymous::__invoke:
     ; (lines=3, args=1, vars=1, tmps=%d)
     ; (after optimizer)
     ; %s.php:%s
0000 CV0($value) = RECV 1
0001 T1 = ADD CV0($value) int(1)
0002 RETURN T1

C::__construct:
     ; (lines=4, args=0, vars=0, tmps=%d)
     ; (after optimizer)
     ; %s.php:%s
0000 INIT_FCALL 1 %d string("var_dump")
0001 SEND_VAL string("C::__construct") 1
0002 DO_ICALL
0003 RETURN null

C::f:
     ; (lines=3, args=1, vars=1, tmps=%d)
     ; (after optimizer)
     ; %s.php:%s
0000 CV0($value) = RECV 1
0001 T1 = ADD CV0($value) int(1)
0002 RETURN T1
string(12) "get_function"
array(2) {
  [0]=>
  int(2)
  [1]=>
  int(3)
}
string(14) "C::__construct"
array(2) {
  [0]=>
  int(2)
  [1]=>
  int(3)
}
array(2) {
  [0]=>
  int(2)
  [1]=>
  int(3)
}
