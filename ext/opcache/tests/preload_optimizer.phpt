--TEST--
Optimizer may rely on preloaded symbols
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.preload={PWD}/preload_optimizer.inc
opcache.opt_debug_level=0x20000
--EXTENSIONS--
opcache
--SKIPIF--
<?php
if (PHP_OS_FAMILY == 'Windows') die('skip Preloading is not supported on Windows');
?>
--FILE--
<?php
echo foo();
?>
--EXPECTF--
$_main:
     ; (lines=1, args=0, vars=0, tmps=%d)
     ; (after optimizer)
     ; $PRELOAD$:0-0
0000 RETURN null

foo:
     ; (lines=1, args=0, vars=0, tmps=%d)
     ; (after optimizer)
     ; %spreload_optimizer.inc:3-5
0000 RETURN int(42)

$_main:
     ; (lines=1, args=0, vars=0, tmps=%d)
     ; (after optimizer)
     ; %spreload_optimizer.inc:1-6
0000 RETURN int(1)

$_main:
     ; (lines=2, args=0, vars=0, tmps=%d)
     ; (after optimizer)
     ; %spreload_optimizer.php:1-4
0000 ECHO string("42")
0001 RETURN int(1)
42
