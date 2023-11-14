--TEST--
JIT ASSIGN: 014
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
;opcache.jit_debug=257
--EXTENSIONS--
opcache
--FILE--
<?php
function foo() {
    $a = 1;
    $b = array();
    $c = $a = $b;
    var_dump($c);
}
foo();
?>
--EXPECT--
array(0) {
}
