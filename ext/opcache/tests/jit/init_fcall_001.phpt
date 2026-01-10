--TEST--
JIT INIT_FCALL: 001 too deep nesting level
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
--FILE--
<?php
ini_set(ini_set(ini_set(ini_set(ini_set(ini_set(ini_set(ini_set(ini_set(
ini_set(ini_set(ini_set(ini_set(ini_set(ini_set(ini_set(ini_set(ini_set(
ini_set(ini_set(ini_set(ini_set(ini_set(ini_set(ini_set(ini_set(ini_set(
ini_set(ini_set(ini_set(ini_set(ini_set(ini_set(
)))))))))))))))))))))))))))))))));
?>
--EXPECTF--
Fatal error: Uncaught ArgumentCountError: ini_set() expects exactly 2 arguments, 0 given in %sinit_fcall_001.php:5
Stack trace:
#0 %sinit_fcall_001.php(5): ini_set()
#1 {main}
  thrown in %sinit_fcall_001.php on line 5

