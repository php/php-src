--TEST--
Test split() function : usage variations  - out-of-range values for limit
--FILE--
<?php
/* Prototype  : proto array split(string pattern, string string [, int limit])
 * Description: Split string into array by regular expression 
 * Source code: ext/standard/reg.c
 * Alias to functions: 
 */

function test_error_handler($err_no, $err_msg, $filename, $linenum, $vars) {
	echo "Error: $err_no - $err_msg, $filename($linenum)\n";
}
set_error_handler('test_error_handler');
echo "*** Testing split() : usage variations ***\n";

$pattern = '[[:space:]]';
$string = '1 2 3 4 5';
var_dump(split($pattern, $string, 0));
var_dump(split($pattern, $string, -10));


echo "Done";
?>
--EXPECTF--
*** Testing split() : usage variations ***
Error: 8192 - Function split() is deprecated, %s(16)
array(1) {
  [0]=>
  string(9) "1 2 3 4 5"
}
Error: 8192 - Function split() is deprecated, %s(17)
array(1) {
  [0]=>
  string(9) "1 2 3 4 5"
}
Done
