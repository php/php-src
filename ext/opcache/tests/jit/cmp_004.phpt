--TEST--
JIT CMP: 004 Comparisons inside conditional statement
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
opcache.protect_memory=1
--EXTENSIONS--
opcache
--FILE--
<?php
function foo(bool $test, int $x) {
    if (($test ? $x >= 1 : $x > 1)) {
        return 1;
    }
    return 0;
}
var_dump(foo(true, 9));
?>
--EXPECT--
int(1)
