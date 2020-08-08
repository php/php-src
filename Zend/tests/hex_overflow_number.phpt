--TEST--
Hex overflow in numeric literal warning
--FILE--
<?php
var_dump(eval('return 0xffff_ffff_ffff_f800;'));
var_dump(eval('return 0xffff_ffff_ffff_fa00;'));
var_dump(eval('return 0xffff_ffff_ffff_fb00;'));
var_dump(eval('return 0xffff_ffff_ffff_ffff;'));
var_dump(eval('return 0x1_ffff_ffff_ffff_ffff;'));
var_dump(eval('return 0x3_ffff_ffff_ffff_ffff;'));
var_dump(eval('return 0x5_ffff_ffff_ffff_ffff;'));
var_dump(eval('return 0x8_ffff_ffff_ffff_ffff;'));
var_dump(eval('return 0x0008_ffff_ffff_ffff_ffff;'));
--EXPECTF--
float(1.844674407370955E+19)

Warning: Saw imprecise float hex literal - the last 2 non-zero bits were truncated in %shex_overflow_number.php(3) : eval()'d code on line 1
float(1.844674407370955E+19)

Warning: Saw imprecise float hex literal - the last 3 non-zero bits were truncated in %shex_overflow_number.php(4) : eval()'d code on line 1
float(1.844674407370955E+19)

Warning: Saw imprecise float hex literal - the last 11 non-zero bits were truncated in %shex_overflow_number.php(5) : eval()'d code on line 1
float(1.8446744073709552E+19)

Warning: Saw imprecise float hex literal - the last 12 non-zero bits were truncated in %shex_overflow_number.php(6) : eval()'d code on line 1
float(3.6893488147419103E+19)

Warning: Saw imprecise float hex literal - the last 13 non-zero bits were truncated in %shex_overflow_number.php(7) : eval()'d code on line 1
float(7.378697629483821E+19)

Warning: Saw imprecise float hex literal - the last 14 non-zero bits were truncated in %shex_overflow_number.php(8) : eval()'d code on line 1
float(1.1068046444225731E+20)

Warning: Saw imprecise float hex literal - the last 15 non-zero bits were truncated in %shex_overflow_number.php(9) : eval()'d code on line 1
float(1.6602069666338596E+20)

Warning: Saw imprecise float hex literal - the last 15 non-zero bits were truncated in %shex_overflow_number.php(10) : eval()'d code on line 1
float(1.6602069666338596E+20)
