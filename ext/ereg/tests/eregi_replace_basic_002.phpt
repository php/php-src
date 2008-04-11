--TEST--
Test eregi_replace() function : basic functionality - a few non-matches
--FILE--
<?php
/* Prototype  : proto string eregi_replace(string pattern, string replacement, string string)
 * Description: Replace regular expression 
 * Source code: ext/standard/reg.c
 * Alias to functions: 
 */

$replacement = 'r';

var_dump(eregi_replace(b'[A-Z]', $replacement, '0'));
var_dump(eregi_replace(b'(a){4}', $replacement, 'aaa'));
var_dump(eregi_replace(b'^a', $replacement, 'ba'));
var_dump(eregi_replace(b'b$', $replacement, 'ba'));
var_dump(eregi_replace(b'[:alpha:]', $replacement, 'x'));


echo "Done";
?>
--EXPECTF--
string(1) "0"
string(3) "aaa"
string(2) "ba"
string(2) "ba"
string(1) "x"
Done