--TEST--
Range inference 016:
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--FILE--
<?php
function test() {
    $a = $y = null;
    for(;;$a-- + $a % $a = $y + $a) {
        var_dump($a);
    }
}
try {
    @test();
} catch (Throwable $e) {
	echo $e->getMessage(), "\n";
}
?>
--EXPECT--
NULL
Modulo by zero
