--TEST--
JIT MUL: 010 incorrect guard elimination
--INI--
opcache.enable=1
opcache.enable_cli=1
--FILE--
<?php
function test() {
    $j = 0;
    for ($i = 0; $i < 40; $i++) {
        $a * $e == $a - $a + $e;
        $maq[$obj] = $a + $e;
        $maq[$obj] = $maq[$obj] = !!$a = $a . 
            $maq[$obj] = $maq[$obj] = $maq[$obj] = $maq[$obj] = $a = $a + $a = &$a +
            $maq[$obj] = $maq[$obj] = $a + $e;
        $maq[$obj] = $maq[$obj] = !!$a = $a . $maq[$obj] = $maq[$obj] = $maq[$obj] =
            $maq[$obj] = $a * $e == $a - $a + $e;
        $maq[$obj] = $maq[$obj] = +$e;
        $a * $e == $a - $a + $e;
        +$e;
        $a * $a = $a + $a = &$a + $e = $a-- +$a + $e;
        $maq[$obj] = $maq[$obj] = !!$a = $a . $a &= $aZ = $a;
    }
}
try {
    @test();
} catch (Throwable $ex) {
	echo $ex->getMessage() . "\n";
}
?>
--EXPECT--
Unsupported operand types: string * float
