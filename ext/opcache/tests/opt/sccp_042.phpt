--TEST--
SCCP 042: Optimisation for CTE calls with named arguments
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=0xE0
opcache.opt_debug_level=0x400000
--FILE--
<?php

print_r(array_keys(array: [1 => 1], strict: true, filter_value: 0));
print_r(array_keys(array: [1 => 1], filter_value: 0, strict: true));
print_r(array_keys(strict: true, filter_value: 1, array: [1 => 1, 2 => 1, 3 => 9]));
print_r(array_keys([1 => 1, 2 => 1, 3 => 9], 1, true));

// The first one will already throw a fatal error.
// We can't put try-catch around these because then it won't optimize.
// We use opcache.opt_debug_level to show us the resulting SSA to verify the CTE code did the right thing.
print_r(array_keys([], strict: true));
print_r(array_keys(array: [], filter_value: 0, array: [1]));
print_r(array_keys(array: [], test: 0, strict: true));

// No CTE possible
$generated = mt_rand(0, 10);
print_r(array_keys(array: [$generated], filter_value: $generated, strict: true));

?>
--EXPECTF--
$_main:
     ; (lines=53, args=0, vars=1, tmps=19, ssa_vars=12, no_loops)
     ; (after dfa pass)
     ; %s
     ; return  [long] RANGE[1..1]
     ; #0.CV0($generated) [undef, ref, any]
BB0:
     ; start exit lines=[0-52]
     ; level=0
0000 INIT_FCALL 1 %d string("print_r")
0001 SEND_VAL array(...) 1
0002 DO_ICALL
0003 INIT_FCALL 1 %d string("print_r")
0004 SEND_VAL array(...) 1
0005 DO_ICALL
0006 INIT_FCALL 1 %d string("print_r")
0007 SEND_VAL array(...) 1
0008 DO_ICALL
0009 INIT_FCALL 1 %d string("print_r")
0010 SEND_VAL array(...) 1
0011 DO_ICALL
0012 INIT_FCALL 1 %d string("print_r")
0013 INIT_FCALL 1 %d string("array_keys")
0014 SEND_VAL array(...) 1
0015 SEND_VAL bool(true) string("strict")
0016 CHECK_UNDEF_ARGS
0017 #5.V9 [array [long] of [long, string]] = DO_ICALL
0018 SEND_VAR #5.V9 [array [long] of [long, string]] 1
0019 DO_ICALL
0020 INIT_FCALL 1 %d string("print_r")
0021 INIT_FCALL 2 %d string("array_keys")
0022 SEND_VAL array(...) 1
0023 SEND_VAL int(0) 2
0024 SEND_VAL array(...) string("array")
0025 CHECK_UNDEF_ARGS
0026 #6.V11 [array [long] of [long, string]] = DO_ICALL
0027 SEND_VAR #6.V11 [array [long] of [long, string]] 1
0028 DO_ICALL
0029 INIT_FCALL 1 %d string("print_r")
0030 INIT_FCALL 1 %d string("array_keys")
0031 SEND_VAL array(...) 1
0032 SEND_VAL_EX int(0) string("test")
0033 SEND_VAL bool(true) string("strict")
0034 CHECK_UNDEF_ARGS
0035 #7.V13 [array [long] of [long, string]] = DO_ICALL
0036 SEND_VAR #7.V13 [array [long] of [long, string]] 1
0037 DO_ICALL
0038 INIT_FCALL 2 %d string("mt_rand")
0039 SEND_VAL int(0) 1
0040 SEND_VAL int(10) 2
0041 #8.V15 [long] = DO_ICALL
0042 ASSIGN #0.CV0($generated) [undef, ref, any] -> #9.CV0($generated) [ref, any] #8.V15 [long]
0043 INIT_FCALL 1 %d string("print_r")
0044 INIT_FCALL 3 %d string("array_keys")
0045 #10.T17 [array [long] of [any]] = INIT_ARRAY 1 (packed) #9.CV0($generated) [ref, any] NEXT
0046 SEND_VAL #10.T17 [array [long] of [any]] 1
0047 SEND_VAR #9.CV0($generated) [ref, any] 2
0048 SEND_VAL bool(true) 3
0049 #11.V18 [array [long] of [long, string]] = DO_ICALL
0050 SEND_VAR #11.V18 [array [long] of [long, string]] 1
0051 DO_ICALL
0052 RETURN int(1)
Array
(
)
Array
(
)
Array
(
    [0] => 1
    [1] => 2
)
Array
(
    [0] => 1
    [1] => 2
)

Fatal error: Uncaught ArgumentCountError: array_keys(): Argument #2 ($filter_value) must be passed explicitly, because the default value is not known in %s:%d
Stack trace:
#0 %s(%d): array_keys(Array, NULL, true)
#1 {main}
  thrown in %s on line %d
