--TEST--
Register Alloction 012: Missed type store
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
--FILE--
<?php
function foo($a) {
    $b = $a;
    $b =! $a = $a + $b & $b & $bb = $a = $a + $b & $a;
    $a = $a + $b & $b & $b = $a;
}

for ($i = 0; $i < 3; $i++) {
    @foo(39087589046889428661);
}
?>
DONE
--EXPECTF--
DONE