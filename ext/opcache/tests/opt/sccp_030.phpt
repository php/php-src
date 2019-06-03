--TEST--
SCCP 030: TYPE_CHECK inferred from type inference info
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.opt_debug_level=0
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

var_dump(is_string(sys_get_temp_dir()));

?>
--EXPECT--
bool(true)
