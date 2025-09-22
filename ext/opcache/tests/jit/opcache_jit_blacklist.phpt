--TEST--
Basic usage of opcache_jit_blacklist()
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.protect_memory=1
opcache.jit=tracing
--EXTENSIONS--
opcache
--FILE--
<?php
function foo() {
    $x = 1;
    $x += 0;
    ++$x;
    var_dump($x);
}
opcache_jit_blacklist(foo(...));
foo();
?>
--EXPECT--
int(2)
