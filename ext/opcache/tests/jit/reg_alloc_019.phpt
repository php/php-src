--TEST--
Register Alloction 019: Incorrect type assumption after spilling
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
--FILE--
<?php
function test() {
    for($i = 0; $i < 10; $i++) {
        $x != $y;
        $x =- +$y;
        $x != $y;
        $x = $y;
     }
}
@test();
?>
DONE
--EXPECTF--
DONE
