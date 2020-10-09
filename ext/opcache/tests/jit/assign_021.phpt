--TEST--
JIT ASSIGN: 021
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
    $a = array();
    $b = $a;
    $c = 1;
    $d = $a = $c;
    var_dump($b, $d);
}
foo();
?>
--EXPECT--
array(0) {
}
int(1)
