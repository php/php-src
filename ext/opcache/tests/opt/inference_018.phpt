--TEST--
Range inference 018:
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--FILE--
<?php
function test() {
    for(;;) {
        $a--;
        $a &= $a / $a;
        $a--;
    }
}
try {
    @test();
} catch (Throwable $e) {
	echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Division by zero
