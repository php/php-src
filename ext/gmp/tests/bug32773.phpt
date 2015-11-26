--TEST--
Bug #32773 (binary GMP functions returns unexpected value, when second parameter is int(0))
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip"; ?>
--FILE--
<?php
echo '10 + 0 = ', gmp_strval(gmp_add(10, 0)), "\n";
echo '10 + "0" = ', gmp_strval(gmp_add(10, '0')), "\n";
                                                                                                              
echo gmp_strval(gmp_div(10, 0))."\n";
echo gmp_strval(gmp_div_qr(10, 0))."\n";
                                                                                                              
?>
--EXPECTF--
10 + 0 = 10
10 + "0" = 10

Warning: gmp_div(): Zero operand not allowed in %s on line %d
0

Warning: gmp_div_qr(): Zero operand not allowed in %s on line %d
0
