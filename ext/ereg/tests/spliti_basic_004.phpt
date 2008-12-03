--TEST--
Test spliti() function : basic functionality - confirm case insensitivity
--FILE--
<?php
/* Prototype  : proto array spliti(string pattern, string string [, int limit])
 * Description: spliti string into array by regular expression 
 * Source code: ext/standard/reg.c
 * Alias to functions: 
 */

$replacement = 'r';

var_dump(spliti('[a-z]', '--- A ---'));
var_dump(spliti('[A-Z]', '--- a ---'));
var_dump(spliti('[[:lower:]]', '--- A ---'));
var_dump(spliti('[[:upper:]]', '--- a ---'));

echo "Done";
?>
--EXPECTF--
array(2) {
  [0]=>
  string(4) "--- "
  [1]=>
  string(4) " ---"
}
array(2) {
  [0]=>
  string(4) "--- "
  [1]=>
  string(4) " ---"
}
array(2) {
  [0]=>
  string(4) "--- "
  [1]=>
  string(4) " ---"
}
array(2) {
  [0]=>
  string(4) "--- "
  [1]=>
  string(4) " ---"
}
Done