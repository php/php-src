--TEST--
JIT ADD: 014 incorrect guard elimination
--INI--
opcache.enable=1
opcache.enable_cli=1
--FILE--
<?php
function test() {
	$j = 0;
    for ($i = 0; $i < 41; $i++) {
        $a &= $a &= $aZ - $a;
        $aZ = $a;
        $a &= $a &= $aZ = $a;
        $aZ = $a;
        $a = &$a + $e = $a-- +$a &= $a &= $a &= $aZ = $a;
    }
}
@test();
?>
DONE
--EXPECT--
DONE