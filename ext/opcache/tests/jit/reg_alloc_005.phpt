--TEST--
Register Alloction 005: Incorrect double/long register hinting
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
--FILE--
<?php
function test() {
	$j = 0;
    for ($i = 1; $i < 10; $i++) {
        $i = 0.0;
        $j++;
        if ($j > 10) break;
    }
}
test();
?>
DONE
--EXPECT--
DONE