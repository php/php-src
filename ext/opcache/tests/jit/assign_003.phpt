--TEST--
JIT ASSIGN: 003
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
;opcache.jit_debug=257
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
function foo() {
    $a = array();  // [rc1, array]
    $a = 1;        // [rc1, long, reg]
    $x = $a;
    var_dump($x);
}
foo();
?>
--EXPECT--
int(1)
