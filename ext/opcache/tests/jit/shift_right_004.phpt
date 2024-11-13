--TEST--
JIT Shift Right: 004
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.protect_memory=1
--FILE--
<?php
function test() {
	$j = 2;
    for ($i = 0; $i < 10;
    	$i + $b = $a + $a = $a + $a = $a +
		    $a = !$a +
			$c[0] .= 0xfff0001/34028236692903846346336*6) {
	    $a =!$a + $a &= 74444444 - 444 >> 4 - $j++;
        if ($j > 14) break;
    }
}
test();
?>
--EXPECTF--
Warning: Undefined variable $a in %sshift_right_004.php on line 8

Warning: Undefined variable $a in %sshift_right_004.php on line 8

Warning: Undefined variable $c in %sshift_right_004.php on line 7

Warning: Undefined array key 0 in %sshift_right_004.php on line 7

Deprecated: Implicit conversion from float %f to int loses precision in %sshift_right_004.php on line 8

Warning: A non-numeric value encountered in %sshift_right_004.php on line 7

Warning: A non-numeric value encountered in %sshift_right_004.php on line 7

Fatal error: Uncaught ArithmeticError: Bit shift by negative number in %sshift_right_004.php:8
Stack trace:
#0 %sshift_right_004.php(12): test()
#1 {main}
  thrown in %sshift_right_004.php on line 8
