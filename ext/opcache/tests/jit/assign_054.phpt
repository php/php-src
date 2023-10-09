--TEST--
JIT ASSIGN: missing type store
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
opcache.protect_memory=1
--FILE--
<?php
function foo($a) {
    $b = $a = $a + $b & $a += $a;
    $b = $a = $a + $b & $b & $b = $a = $a + $b = $a = $a + $b = $a += $a;
    $b = !$a = $a + $b & $b & $b = $b = $a = $a + $b & $a += $a;
	$a + $b & $b & $b = $a = $a + $b = $a = $a + $b = $a += $a;
}

@foo(39087589046889428661);
@foo(390875890468877606478);
@foo(390875890468877606478);
?>
DONE
--EXPECT--
DONE
