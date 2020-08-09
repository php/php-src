--TEST--
JIT INC: 019
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
opcache.protect_memory=1
;opcache.jit_debug=257
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
function bar($b) {
    if ($b) {
        $a = 1;
    } else {
        $a = 2;
    }
    isset($a);
    var_dump($a++);
    return $a;
}
var_dump(bar(0));
?>
--EXPECT--
int(2)
int(3)
