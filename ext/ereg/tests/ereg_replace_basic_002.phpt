--TEST--
Test ereg_replace() function : basic functionality - a few non-matches
--FILE--
<?php
/* Prototype  : proto string ereg_replace(string pattern, string replacement, string string)
 * Description: Replace regular expression 
 * Source code: ext/standard/reg.c
 * Alias to functions: 
 */

$replacement = 'r';

var_dump(ereg_replace('A', $replacement, 'a'));
var_dump(ereg_replace('[A-Z]', $replacement, '0'));
var_dump(ereg_replace('(a){4}', $replacement, 'aaa'));
var_dump(ereg_replace('^a', $replacement, 'ba'));
var_dump(ereg_replace('b$', $replacement, 'ba'));
var_dump(ereg_replace('[:alpha:]', $replacement, 'x'));


echo "Done";
?>
--EXPECTF--
string(1) "a"
string(1) "0"
string(3) "aaa"
string(2) "ba"
string(2) "ba"
string(1) "x"
Done