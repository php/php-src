--TEST--
zend_version() should be evaluated at compile time
--EXTENSIONS--
opcache
--INI--
opcache.enable_cli=1
opcache.opt_debug_level=0x20000
--FILE--
<?php

$v = zend_version();
var_dump($v);

?>
--EXPECTF--
$_main:
     ; (lines=7, args=0, vars=1, tmps=%d)
     ; (after optimizer)
     ; %s.php:%s
0000 INIT_FCALL 0 %d string("zend_version")
0001 T1 = DO_ICALL
0002 ASSIGN CV0($v) T1
0003 INIT_FCALL 1 %d string("var_dump")
0004 SEND_VAR CV0($v) 1
0005 DO_ICALL
0006 RETURN int(1)
string(%d) "%s"
