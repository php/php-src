--TEST--
JIT ADD: 013 register allocation (incorrect hinting)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
--FILE--
<?php
function y(){
    $j = 2;
    for (; $a = $j - 7 + $y = $a - 7; $a = $a + 1 / 3) {
        $j++;
        if ($j > 4) break;
    }
}
?>
DONE
--EXPECT--
DONE