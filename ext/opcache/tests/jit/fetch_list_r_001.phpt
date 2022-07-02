--TEST--
JIT FETCH_LIST_R: 001
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
--FILE--
<?php
function test() {
    $v = 'a';
    list($$v) = "";
    var_dump($a);
}
test();
?>
--EXPECT--
NULL
