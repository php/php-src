--TEST--
Register Alloction 014: Register clobbering
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
--FILE--
<?php
function foo() {
    for($cnt = 0; $cnt < 6; $cnt++) {
        $e = $a-- + $a-- + $a *= $a;
        for ($i = 0; $i <= .1; $i++);
    }
}
foo();
?>
DONE
--EXPECTF--
Warning: Undefined variable $a in %sreg_alloc_014.php on line 4
DONE