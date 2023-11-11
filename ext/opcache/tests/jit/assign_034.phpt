--TEST--
JIT ASSIGN: 034
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
opcache.protect_memory=1
;opcache.jit_debug=257
--EXTENSIONS--
opcache
--FILE--
<?php
function bar() {
    $a = strlen("a");
    $a++;
    return $a;
}
var_dump(bar());
?>
--EXPECT--
int(2)
