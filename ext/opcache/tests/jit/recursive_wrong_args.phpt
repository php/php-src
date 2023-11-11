--TEST--
Recursive call with too few args
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
--FILE--
<?php
function f($arg) {
    f();
}
try {
    f();
} catch (ArgumentCountError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECTF--
Too few arguments to function f(), 0 passed in %s on line %d and exactly 1 expected
