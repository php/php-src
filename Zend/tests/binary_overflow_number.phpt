--TEST--
Octal overflow in numeric literal warning
--FILE--
<?php
// rounding down
var_dump(eval('return 0b1011111111111111111111111111111111111111111111111111100000000000;'));
var_dump(eval('return 0b1011111111111111111111111111111111111111111111111111100000000001;'));
// rounding up
var_dump(eval('return 0b1011111111111111111111111111111111111111111111111111111111111111;'));
var_dump(eval('return 0b1011111111111111111111111111111111111111111111111111110000000000;'));
var_dump(eval('return 0b1011111111111111111111111111111111111111111111111111111000000000;'));
// don't count _ or leading 0s
var_dump(eval('return     0b111_111_111_111_111_111_111_111_111_111_111_111_111_111_111_111_111_111_000_000_000_0;'));
var_dump(eval('return 0b000_111_111_111_111_111_111_111_111_111_111_111_111_111_111_111_111_111_111_000_000_000_0;'));
var_dump(eval('return 0b1111111111111111111111111111111111111111111111111111111111111111;'));
var_dump(eval('return 0b1000000000000000000000000000000000000000000000000000010000000000;'));
--EXPECTF--
float(1.383505805528216E+19)

Warning: Saw imprecise float binary literal - the last 11 non-zero bits were truncated in %sbinary_overflow_number.php(4) : eval()'d code on line 1
float(1.383505805528216E+19)

Warning: Saw imprecise float binary literal - the last 11 non-zero bits were truncated in %sbinary_overflow_number.php(6) : eval()'d code on line 1
float(1.3835058055282164E+19)

Warning: Saw imprecise float binary literal - the last 1 non-zero bits were truncated in %sbinary_overflow_number.php(7) : eval()'d code on line 1
float(1.3835058055282164E+19)

Warning: Saw imprecise float binary literal - the last 2 non-zero bits were truncated in %sbinary_overflow_number.php(8) : eval()'d code on line 1
float(1.3835058055282164E+19)

Warning: Saw imprecise float binary literal - the last 1 non-zero bits were truncated in %sbinary_overflow_number.php(10) : eval()'d code on line 1
float(1.844674407370955E+19)

Warning: Saw imprecise float binary literal - the last 1 non-zero bits were truncated in %sbinary_overflow_number.php(11) : eval()'d code on line 1
float(1.844674407370955E+19)

Warning: Saw imprecise float binary literal - the last 11 non-zero bits were truncated in %sbinary_overflow_number.php(12) : eval()'d code on line 1
float(1.844674407370955E+19)

Warning: Saw imprecise float binary literal - the last 1 non-zero bits were truncated in %sbinary_overflow_number.php(13) : eval()'d code on line 1
float(9.223372036854775E+18)