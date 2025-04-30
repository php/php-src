--TEST--
Register Alloction 017: Missing store
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
--FILE--
<?php
function test() {
    for($i = 0; $i < 10; $i++) {
        $a = $b = $a + !$a = !$a;
        $c = $c = $a;
        $c % $a;
    }
}
@test();
?>
DONE
--EXPECT--
DONE
