--TEST--
JIT ASSIGN: 001
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
    $c = $a;
    $a = 1;
    $x = $a;
    var_dump($x, $b, $c);
}
foo();
?>
--EXPECT--
int(1)
array(0) {
}
array(0) {
}
