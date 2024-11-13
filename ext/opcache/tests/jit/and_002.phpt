--TEST--
JIT BW_AND: 002 (memory leak)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
--FILE--
<?php
function t($a) {
    for($i = 0; $i < 2; $i++)
        $a &= $a = $a;
    }
t("");
?>
DONE
--EXPECTF--
DONE
