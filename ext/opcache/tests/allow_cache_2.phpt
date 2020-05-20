--TEST--
allow_cache_2: allow_cache=0 takes precedence over file_cache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_cache_only=1
opcache.file_cache="{TMP}"
opcache.allow_cache=0
opcache.opt_debug_level=0x20000
opcache.optimization_level=-1
opcache.preload=
--SKIPIF--
<?php require_once('skipif.inc'); ?>
<?php if (PHP_OS_FAMILY == 'Windows') die('skip..  not for Windows'); ?>
--FILE--
<?php
// Opcache should actually run.
// Because allow_cache is used, this will consistently emit debug output as a side effect.
// It should also indicate that file_cache is not used.
$status = opcache_get_status();
var_dump($status);
?>
--EXPECTF--
$_main:
     ; (lines=7, args=0, vars=1, tmps=1)
     ; (after optimizer)
     ; %sallow_cache_2.php:1-8
0000 INIT_FCALL 0 %d string("opcache_get_status")
0001 V1 = DO_ICALL
0002 ASSIGN CV0($status) V1
0003 INIT_FCALL 1 %d string("var_dump")
0004 SEND_VAR CV0($status) 1
0005 DO_ICALL
0006 RETURN int(1)
array(3) {
  ["optimizations_enabled"]=>
  bool(true)
  ["opcache_enabled"]=>
  bool(false)
  ["allow_cache"]=>
  bool(false)
}
