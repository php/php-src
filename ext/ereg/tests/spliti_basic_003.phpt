--TEST--
Test spliti() function : basic functionality - a few non-matches
--FILE--
<?php
/* Prototype  : proto array spliti(string pattern, string string [, int limit])
 * Description: spliti string into array by regular expression 
 * Source code: ext/standard/reg.c
 * Alias to functions: 
 */

$replacement = 'r';

var_dump(spliti('[A-Z]', '-- 0 --'));
var_dump(spliti('(a){4}', '--- aaa ---'));
var_dump(spliti('^a', '--- ba ---'));
var_dump(spliti('b$', '--- ba ---'));
var_dump(spliti('[:alpha:]', '--- x ---'));


echo "Done";
?>
--EXPECTF--
array(1) {
  [0]=>
  string(7) "-- 0 --"
}
array(1) {
  [0]=>
  string(11) "--- aaa ---"
}
array(1) {
  [0]=>
  string(10) "--- ba ---"
}
array(1) {
  [0]=>
  string(10) "--- ba ---"
}
array(1) {
  [0]=>
  string(9) "--- x ---"
}
Done