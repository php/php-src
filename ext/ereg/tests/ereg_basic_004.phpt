--TEST--
Test ereg() function : basic functionality - a few non-matches
--FILE--
<?php
/* Prototype  : proto int ereg(string pattern, string string [, array registers])
 * Description: Regular expression match 
 * Source code: ext/standard/reg.c
 * Alias to functions: 
 */

$regs = 'original';

var_dump(ereg('A', 'a', $regs));
var_dump(ereg('[A-Z]', '0', $regs));
var_dump(ereg('(a){4}', 'aaa', $regs));
var_dump(ereg('^a', 'ba', $regs));
var_dump(ereg('b$', 'ba', $regs));
var_dump(ereg('[:alpha:]', 'x', $regs));

// Ensure $regs is unchanged
var_dump($regs);

echo "Done";
?>
--EXPECTF--
Deprecated: Function ereg() is deprecated in %s on line %d
bool(false)

Deprecated: Function ereg() is deprecated in %s on line %d
bool(false)

Deprecated: Function ereg() is deprecated in %s on line %d
bool(false)

Deprecated: Function ereg() is deprecated in %s on line %d
bool(false)

Deprecated: Function ereg() is deprecated in %s on line %d
bool(false)

Deprecated: Function ereg() is deprecated in %s on line %d
bool(false)
string(8) "original"
Done
