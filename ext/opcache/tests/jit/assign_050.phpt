--TEST--
JIT ASSIGN: incorrect type store elimination
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
opcache.protect_memory=1
--FILE--
<?php
function foo($a) {
    $b = $a;
    $b =! $a = $a + $b & $b & $b = $b = $a = $a + $b & $a += $a;
    $b = $b = $a = $a + $b & $b & $b = $a = $a + $b = $b = $a = $a + $b = $a += $a;
}

for ($i = 0; $i < 3; $i++) {
    @foo(39087589046889428661);
}
?>
DONE
--EXPECT--
DONE
