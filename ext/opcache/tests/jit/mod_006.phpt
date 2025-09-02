--TEST--
JIT MOD: 005
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.protect_memory=1
--FILE--
<?php
function foo(){
    $a = 1;
    $b = $a % 0;
    yield $b;
}
?>
DONE
--EXPECT--
DONE