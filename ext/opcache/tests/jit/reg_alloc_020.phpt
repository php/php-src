--TEST--
Register Alloction 020: Incorrect TSSA type inference
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
--FILE--
<?php
function foo() {
    for($j=0; $j<6; $j++) {
        $y = $a;
        $a = 6;
        for(;$i;);
    }
}
@foo();
?>
DONE
--EXPECTF--
DONE
