--TEST--
Bug #81272: Segfault in var[] after array_slice with JIT
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit_buffer_size=16M
opcache.jit=function
--FILE--
<?php

function test() {
    $newPages = array_slice([], 0, 0);
    $newPages[] = null;
}

test();

?>
===DONE===
--EXPECT--
===DONE===
