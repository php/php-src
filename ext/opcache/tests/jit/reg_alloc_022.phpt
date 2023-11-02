--TEST--
Register Alloction 022: Incorrect type assumption
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
--FILE--
<?php
function test() {
	$a = $b = $x = 0; $c = null;
    for ($i = 0; $i < 20; $i++) {
        $x .= $b;
        $x = $a ? $b : $c;
        $a &= $x != $a ? $b : $c;
        $x = $a ? $b : $c;
        $a &= $x != $a ? $b : $c;
        $x != $a ?: $c;
        $a--;
    }
}
test();
?>
DONE
--EXPECT--
DONE