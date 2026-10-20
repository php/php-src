--TEST--
SCCP 042: SPACESHIP is evaluated for constant operands
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
function test() {
    $a = 1;
    $b = 2;
    return [$a <=> $b, $b <=> $a];
}
var_dump(test());
?>
--EXPECTF--
$_main:
     ; (lines=6, args=0, vars=0, tmps=%d)
     ; (after optimizer)
     ; %s
0000 INIT_FCALL 1 %d string("var_dump")
0001 INIT_FCALL 0 %d string("test")
0002 T0 = DO_UCALL
0003 SEND_VAL T0 1
0004 DO_ICALL
0005 RETURN int(1)

test:
     ; (lines=1, args=0, vars=0, tmps=%d)
     ; (after optimizer)
     ; %s
0000 RETURN array(...)
array(2) {
  [0]=>
  int(-1)
  [1]=>
  int(1)
}
