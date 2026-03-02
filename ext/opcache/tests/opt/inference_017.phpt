--TEST--
Range inference 017:
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--FILE--
<?php
function test() {
    for(;;) {
        $a = $a-- + 0 / $a;
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
