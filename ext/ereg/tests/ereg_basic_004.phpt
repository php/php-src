--TEST--
Test ereg() function : basic functionality - a few non-matches
--FILE--
<?php
/* Prototype  : proto int ereg(string pattern, string string [, array registers])
 * Description: Regular expression match 
 * Source code: ext/standard/reg.c
 * Alias to functions: 
 */

$regs = b'original';

var_dump(ereg(b'A', b'a', $regs));
var_dump(ereg(b'[A-Z]', b'0', $regs));
var_dump(ereg(b'(a){4}', b'aaa', $regs));
var_dump(ereg(b'^a', b'ba', $regs));
var_dump(ereg(b'b$', b'ba', $regs));
var_dump(ereg(b'[:alpha:]', b'x', $regs));

// Ensure $regs is unchanged
var_dump($regs);

echo "Done";
?>
--EXPECTF--
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
string(8) "original"
Done