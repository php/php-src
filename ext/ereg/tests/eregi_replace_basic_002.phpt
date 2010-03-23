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

var_dump(eregi_replace('[A-Z]', $replacement, '0'));
var_dump(eregi_replace('(a){4}', $replacement, 'aaa'));
var_dump(eregi_replace('^a', $replacement, 'ba'));
var_dump(eregi_replace('b$', $replacement, 'ba'));
var_dump(eregi_replace('[:alpha:]', $replacement, 'x'));


echo "Done";
?>
--EXPECTF--
Deprecated: Function eregi_replace() is deprecated in %s on line %d
string(1) "0"

Deprecated: Function eregi_replace() is deprecated in %s on line %d
string(3) "aaa"

Deprecated: Function eregi_replace() is deprecated in %s on line %d
string(2) "ba"

Deprecated: Function eregi_replace() is deprecated in %s on line %d
string(2) "ba"

Deprecated: Function eregi_replace() is deprecated in %s on line %d
string(1) "x"
Done
