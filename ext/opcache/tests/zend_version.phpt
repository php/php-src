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
     ; (lines=5, args=0, vars=1, tmps=%d)
     ; (after optimizer)
     ; %s.php:%s
0000 ASSIGN CV0($v) string("%s")
0001 INIT_FCALL 1 %d string("var_dump")
0002 SEND_VAR CV0($v) 1
0003 DO_ICALL
0004 RETURN int(1)
string(%d) "%s"
