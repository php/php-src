--TEST--
Test mb_decode_mimeheader() function : error conditions
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_decode_mimeheader') or die("skip mb_decode_mimeheader() is not available in this build");
?>
--FILE--
<?php
/* Prototype  : string mb_decode_mimeheader(string string)
 * Description: Decodes the MIME "encoded-word" in the string
 * Source code: ext/mbstring/mbstring.c
 * Alias to functions:
 */

echo "*** Testing mb_decode_mimeheader() : error conditions ***\n";

// Zero arguments
echo "\n-- Testing mb_decode_mimeheader() function with Zero arguments --\n";
var_dump( mb_decode_mimeheader() );

//Test mb_decode_mimeheader with one more than the expected number of arguments
echo "\n-- Testing mb_decode_mimeheader() function with more than expected no. of arguments --\n";
$string = 'string_val';
$extra_arg = 10;
var_dump( mb_decode_mimeheader($string, $extra_arg) );

?>
===DONE===
--EXPECTF--
*** Testing mb_decode_mimeheader() : error conditions ***

-- Testing mb_decode_mimeheader() function with Zero arguments --

Warning: mb_decode_mimeheader() expects exactly 1 parameter, 0 given in %s on line %d
NULL

-- Testing mb_decode_mimeheader() function with more than expected no. of arguments --

Warning: mb_decode_mimeheader() expects exactly 1 parameter, 2 given in %s on line %d
NULL
===DONE===
