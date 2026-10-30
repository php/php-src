--TEST--
Range inference 019:
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--FILE--
<?php
function test() {
    $a = $y = null;
    for($cnt = 0; $cnt < 6; $cnt++) {
        $e = $a-- + $a *= $a;
        $a-- + $y -= $e;
        $a-- + $a *= $a;
        $a-- + $a *= $a;
    }
}
test();
?>
DONE
--EXPECTF--

Warning: Decrement on type null has no effect, this will change in the next major version of PHP in %sinference_019.php on line %d
DONE
