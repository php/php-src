--TEST--
Octal overflow in numeric literal warning
--FILE--
<?php
var_dump(eval('return     01777777777777777770000;'));
var_dump(eval('return 0_00_1777777777777777770000;'));
var_dump(eval('return     01777777777777777772000;'));
var_dump(eval('return     01777777777777777774000;'));
var_dump(eval('return     02777777777777777774000;'));
var_dump(eval('return     07777777777777777774000;'));
var_dump(eval('return 04_777_777_7777777777774000;'));
--EXPECTF--
float(1.8446744073709548E+19)
float(1.8446744073709548E+19)

Warning: Saw imprecise float octal literal - the last 1 non-zero bits were truncated in %soctal_overflow_number.php(4) : eval()'d code on line 1
float(1.8446744073709548E+19)
float(1.8446744073709552E+19)

Warning: Saw imprecise float octal literal - the last 1 non-zero bits were truncated in %soctal_overflow_number.php(6) : eval()'d code on line 1
float(2.7670116110564327E+19)

Warning: Saw imprecise float octal literal - the last 2 non-zero bits were truncated in %soctal_overflow_number.php(7) : eval()'d code on line 1
float(7.378697629483821E+19)

Warning: Saw imprecise float octal literal - the last 5 non-zero bits were truncated in %soctal_overflow_number.php(8) : eval()'d code on line 1
float(3.68934881474191E+20)