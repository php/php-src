--TEST--
Test JIT information in opcache_get_status
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
opcache.protect_memory=1
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

var_dump(opcache_get_status()['jit'])

?>
--EXPECTF--
array(9) {
  ["enabled"]=>
  bool(true)
  ["on"]=>
  bool(true)
  ["kind"]=>
  int(0)
  ["opt_level"]=>
  int(5)
  ["opt_flags"]=>
  int(6)
  ["buffer_size"]=>
  int(%d)
  ["buffer_free"]=>
  int(%d)
  ["function_compilation_successes"]=>
  int(1)
  ["function_compilation_failures"]=>
  int(0)
}
