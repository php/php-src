--TEST--
Bug #80426: Crash when using JIT and an extension replacing zend_execute_ex with custom
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit_buffer_size=1M
zend_test.replace_zend_execute_ex=1
--SKIPIF--
<?php require_once('skipif.inc'); ?>
<?php if (!isset(opcache_get_status()["jit"])) die('skip: JIT is not available'); ?>
<?php if (!extension_loaded('zend-test')) die('skip: zend-test extension required'); ?>
--FILE--
<?php

function compute() {
    if (true) {
    }
}

for ($i = 0; $i <= 64; $i++) {
    compute();
}

?>
===DONE===
--EXPECT--
Warning: JIT is incompatible with third party extensions that override zend_execute_ex(). JIT disabled. in Unknown on line 0
===DONE===
