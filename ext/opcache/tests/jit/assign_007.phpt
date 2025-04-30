--TEST--
JIT ASSIGN: 007
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
;opcache.jit_debug=257
--EXTENSIONS--
opcache
--FILE--
<?php
function foo() {
    $a = 1.0;
    $c = 2.0;
    $a = 1;
    var_dump($a);
}
foo();
?>
--EXPECT--
int(1)
