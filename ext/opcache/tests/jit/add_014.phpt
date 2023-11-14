--TEST--
JIT ADD: 014 incorrect guard elimination
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
--FILE--
<?php
function test() {
	$s = null;
    $i = $a + $a = $j = 2;
    for ($a = 0; $i < 20; $a = !$a + $s .= 0xfff0001/34028236692903846346336*6) {
	    $a = !$a + $a &= 74444444 - 444 >> 4 - $j++;
        if ($j > 14) break;
    }
}
try {
    @test();
} catch (Throwable $e) {
	echo $e->getMessage() . "\n";
}
?>
--EXPECT--
Bit shift by negative number
