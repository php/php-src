--TEST--
JIT ASSIGN: 008
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
;opcache.jit_debug=257
--EXTENSIONS--
opcache
--FILE--
<?php
function foo() {
    $a = 1.0;
    $c = 2.0;
    $c = $a;
    var_dump($a);
}
foo();
?>
--EXPECT--
float(1)
