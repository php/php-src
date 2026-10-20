--TEST--
array_map(): foreach optimization requires strict_types=0 (GH-23882)
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.opt_debug_level=0x20000
--FILE--
<?php

declare(strict_types=1);

var_dump(array_map(trim(...), [1]));

?>
--EXPECTF--
$_main:
     ; (lines=%d, args=0, vars=%d, tmps=%d)
     ; (after optimizer)
     ; %s
0000 INIT_FCALL 1 %d string("var_dump")
0001 INIT_FCALL 2 %d string("array_map")
0002 INIT_FCALL 0 %d string("trim")
0003 T0 = CALLABLE_CONVERT %d
0004 SEND_VAL T0 1
0005 SEND_VAL array(...) 2
0006 T0 = DO_ICALL
0007 SEND_VAL T0 1
0008 DO_ICALL
0009 RETURN int(1)
array(1) {
  [0]=>
  string(1) "1"
}
