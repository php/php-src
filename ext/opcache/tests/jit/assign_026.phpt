--TEST--
JIT ASSIGN: 026
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
    $a = array(1,2,3);
    $b=&$a;
    $b=1;
    $a = new stdClass;
    $a->a=1;
    $a->b=2;
    $b=&$a;
}
foo();
echo "ok\n";
?>
--EXPECT--
ok
