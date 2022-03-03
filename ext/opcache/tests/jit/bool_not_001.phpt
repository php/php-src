--TEST--
JIT BOOL_NOT: 001 Memory leak in case of reference to bool
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
opcache.protect_memory=1
opcache.jit=function
--EXTENSIONS--
opcache
--FILE--
<?php
function test() {
    $a = true;
    var_dump(!$b =& $a);
}
test();
?>
--EXPECT--
bool(false)
