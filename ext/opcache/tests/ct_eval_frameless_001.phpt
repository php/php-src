--TEST--
Test ct eval of frameless function
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.opt_debug_level=0x20000
--EXTENSIONS--
opcache
--FILE--
<?php
echo dirname(__DIR__);
?>
--EXPECTF--
$_main:
     ; (lines=2, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %sct_eval_frameless_001.php:1-4
0000 ECHO string("%sopcache")
0001 RETURN int(1)
%sopcache
