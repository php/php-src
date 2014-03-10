--TEST--
Test spliti() function : usage variations  - out-of-range values for limit
--FILE--
<?php
/* Prototype  : proto array spliti(string pattern, string string [, int limit])
 * Description: spliti string into array by regular expression 
 * Source code: ext/standard/reg.c
 * Alias to functions: 
 */

function test_error_handler($err_no, $err_msg, $filename, $linenum, $vars) {
	echo "Error: $err_no - $err_msg, $filename($linenum)\n";
}
set_error_handler('test_error_handler');
echo "*** Testing spliti() : usage variations ***\n";

$pattern = '[[:space:]]';
$string = '1 2 3 4 5';
var_dump(spliti($pattern, $string, 0));
var_dump(spliti($pattern, $string, -10));


echo "Done";
?>
--EXPECTF--
*** Testing spliti() : usage variations ***
Error: 8192 - Function spliti() is deprecated, %s(16)
array(1) {
  [0]=>
  string(9) "1 2 3 4 5"
}
Error: 8192 - Function spliti() is deprecated, %s(17)
array(1) {
  [0]=>
  string(9) "1 2 3 4 5"
}
Done
