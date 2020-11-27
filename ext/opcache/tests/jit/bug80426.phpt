--TEST--
Bug #80426: Crash when using JIT and an extension replacing zend_execute_ex with custom
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit_buffer_size=1M
zend_test.replace_zend_execute_ex=1
--SKIPIF--
<?php require_once('skipif.inc'); ?>
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
===DONE===
