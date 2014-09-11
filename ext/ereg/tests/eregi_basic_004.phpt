--TEST--
Test eregi() function : basic functionality - a few non-matches
--FILE--
<?php
/* Prototype  : proto int eregi(string pattern, string string [, array registers])
 * Description: Regular expression match 
 * Source code: ext/standard/reg.c
 * Alias to functions: 
 */

$regs = 'original';

var_dump(eregi('[A-Z]', '0', $regs));
var_dump(eregi('(a){4}', 'aaa', $regs));
var_dump(eregi('^a', 'ba', $regs));
var_dump(eregi('b$', 'ba', $regs));
var_dump(eregi('[:alpha:]', 'x', $regs));

// Ensure $regs is unchanged
var_dump($regs);

echo "Done";
?>
--EXPECTF--
Deprecated: Function eregi() is deprecated in %s on line %d
bool(false)

Deprecated: Function eregi() is deprecated in %s on line %d
bool(false)

Deprecated: Function eregi() is deprecated in %s on line %d
bool(false)

Deprecated: Function eregi() is deprecated in %s on line %d
bool(false)

Deprecated: Function eregi() is deprecated in %s on line %d
bool(false)
string(8) "original"
Done
