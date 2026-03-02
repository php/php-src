--TEST--
GH-16184 (UBSan address overflowed in ext/pcre/php_pcre.c)
--CREDITS--
YuanchengJiang
--FILE--
<?php

$string = 'This is a string. It contains numbers (0*9) as well as parentheses and some other things!';
echo preg_replace(array('/\b\w{1}s/', '/(\d{1})*(\d{1})/', '/[\(!\)]/'), array('test', '$1 to $2', '*'), $string), "\n";

?>
--EXPECT--
This test a string. It contains numbers * to 0* to 9* test well test parentheses and some other things*
