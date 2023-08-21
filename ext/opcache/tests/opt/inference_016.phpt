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
    test();
} catch (Throwable $e) {
	echo $e->getMessage(), "\n";
}
?>
--EXPECTF--
NULL

Warning: Decrement on type null has no effect, this will change in the next major version of PHP in %sinference_016.php on line %d
Modulo by zero
