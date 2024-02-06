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
echo substr('foo', 1, $foo ? 1 : 1);
?>
--EXPECTF--
$_main:
     ; (lines=3, args=0, vars=1, tmps=0)
     ; (after optimizer)
     ; %sct_eval_frameless_002.php:1-4
0000 CHECK_VAR CV0($foo)
0001 ECHO string("o")
0002 RETURN int(1)

Warning: Undefined variable $foo in %s on line %d
o
