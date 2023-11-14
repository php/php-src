--TEST--
JIT ASSIGN: 010
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
    $a = array();
    $b =& $a;
    $a = $b;
}
foo();
echo "ok\n";
?>
--EXPECT--
ok
