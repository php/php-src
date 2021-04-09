--TEST--
JIT INC: 014
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
opcache.protect_memory=1
;opcache.jit_debug=257
--EXTENSIONS--
opcache
--FILE--
<?php
function foo() {
    $x = 1.0;
    var_dump(++$x); // reg -> mem, mem
    var_dump($x);
}
foo();
?>
--EXPECT--
float(2)
float(2)
