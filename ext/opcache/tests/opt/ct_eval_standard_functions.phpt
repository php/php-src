--TEST--
Compile-time evaluation of str_increment() and str_decrement()
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.opt_debug_level=0x20000
zend_test.observer.enabled=0
--FILE--
<?php
function test() {
    return [
        str_increment('Z9'),
        str_decrement('AA0'),
    ];
}
var_dump(test());
?>
--EXPECTF--
$_main:
     ; (lines=6, args=0, vars=0, tmps=1)
     ; (after optimizer)
     ; %sct_eval_standard_functions.php:1-10
0000 INIT_FCALL 1 %d string("var_dump")
0001 INIT_FCALL 0 %d string("test")
0002 T0 = DO_UCALL
0003 SEND_VAL T0 1
0004 DO_ICALL
0005 RETURN int(1)

test:
     ; (lines=1, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %sct_eval_standard_functions.php:2-7
0000 RETURN array(...)
array(2) {
  [0]=>
  string(3) "AA0"
  [1]=>
  string(2) "Z9"
}
