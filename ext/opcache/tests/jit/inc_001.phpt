--TEST--
JIT INC: 001
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.protect_memory=1
;opcache.jit_debug=257
--EXTENSIONS--
opcache
--FILE--
<?php
function foo() {
    $x = 1;
    $x += 0;
    ++$x; // mem -> mem
    var_dump($x);
}
foo();
?>
--EXPECT--
int(2)
