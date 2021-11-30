--TEST--
JIT BOOL_NOT: 002 Incorrect function JIT for MAY_BE_DOUBLE|MAY_BE_UNDEF
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
opcache.protect_memory=1
--FILE--
<?php
function test() {
	$j = 2;
    for ($i = 0; $i < 10;
    	$a = !$i + $c[0] = 0xfff0001/34028236692903846346336*6) {
	    $a =!$a + $a &= 74444444 - 444 >> 4 - $j++;
        if ($j > 14) break;
    }
}
test();
?>
--EXPECTF--
Warning: Undefined variable $a in %sbool_not_002.php on line 6

Warning: Undefined variable $a in %sbool_not_002.php on line 6

Deprecated: Implicit conversion from float %f to int loses precision in %sbool_not_002.php on line 6

Deprecated: Implicit conversion from float %f to int loses precision in %sbool_not_002.php on line 6

Fatal error: Uncaught ArithmeticError: Bit shift by negative number in %sbool_not_002.php:6
Stack trace:
#0 %sbool_not_002.php(10): test()
#1 {main}
  thrown in %sbool_not_002.php on line 6
