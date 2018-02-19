--TEST--
Test preg_replace() function : variation both arguments are arrays
--FILE--
<?php
/*
* proto string preg_replace(mixed regex, mixed replace, mixed subject [, int limit [, count]])
* Function is implemented in ext/pcre/php_pcre.c
*/
/*
* Testing preg_replace when the regex and the replacement are both arrays.
*/
$string = 'This is a string. It contains numbers (0-9) as well as parentheses and some other things!';
$new_string = preg_replace(array('/\b\w{1}s/', '/(\d{1})-(\d{1})/', '/[\(!\)]/'), array('test', '$1 to $2', '*'), $string);
print $new_string;
?>
--EXPECT--
This test a string. It contains numbers *0 to 9* test well test parentheses and some other things*
