--TEST--
Testing str_truncate() function
--FILE--
<?php

/* Prototype: string substr( string str, int start[, int length] )
 * Description: Truncates the string to the new length.
 */

$string = 'Hello world!';

// Should not take effect, new_len is too big.
str_truncate($string, strlen($string) + 1);
var_dump($string);
// Should not take effect, new_len is less than zero.
str_truncate($string, -1);
var_dump($string);

// Should truncate $string to the string "Hello".
str_truncate($string, strlen('Hello'));
var_dump($string);
// Should not take effect, it cannot untruncate back.
str_truncate($string, strlen('Hello world!'));
var_dump($string);
// Should truncate to an empty string.
str_truncate($string, 0);
var_dump($string);
// Should not take effect, it cannot untruncate back.
str_truncate($string, strlen('Hello'));
var_dump($string);

echo "\nDone";

?>
--EXPECTF--
string(12) "Hello world!"
string(12) "Hello world!"
string(5) "Hello"
string(5) "Hello"
string(0) ""
string(0) ""

Done
