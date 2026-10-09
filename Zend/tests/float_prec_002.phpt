--TEST--
Float parsing and arithmetic are correctly rounded (no x87 extended-precision double rounding)
--INI--
serialize_precision=-1
--FILE--
<?php
function f(string $hex): float { return unpack('E', hex2bin($hex))[1]; }
function h(float $f): string { return bin2hex(pack('E', $f)); }
function mul(float $a, float $b): float { return $a * $b; }
function dv(float $a, float $b): float { return $a / $b; }

/* zend_strtod() fast path: one FP multiply/divide by a power of ten */
var_dump(343034272.826839, 8119.30762886814, 243734239727298e17, 185013812864946e11);
var_dump(h((float) "343034272.826839"), h((float) "8119.30762886814"));

/* VM arithmetic, operands built at runtime so nothing is constant-folded */
var_dump(h(mul(f('3ff5822e489af57e'), f('3ff69399f58e692c'))));
var_dump(h(dv(f('3ffae2b2205022a1'), f('3ff6176d521a6fcb'))));
?>
--EXPECT--
float(343034272.826839)
float(8119.30762886814)
float(2.43734239727298E+31)
float(1.85013812864946E+25)
string(16) "41b47249a0d3abb9"
string(16) "40bfb74ec0c3f7f7"
string(16) "3ffe596aa4038cfb"
string(16) "3ff378dc9d02d895"
