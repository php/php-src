--TEST--
PFAs in constants declared on the same line must not share a cache entry
--FILE--
<?php
function f($a, $b) { return "$a|$b"; }

class C { const X = f(?, 'x'), Y = f('y', ?); }
const K1 = f(?, 'k1'), K2 = f('k2', ?);

var_dump((C::X)(1), (C::Y)(2), (K1)(3), (K2)(4));
?>
--EXPECT--
string(3) "1|x"
string(3) "y|2"
string(4) "3|k1"
string(4) "k2|4"
