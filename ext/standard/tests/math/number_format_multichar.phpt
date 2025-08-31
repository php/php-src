--TEST--
Test number_format() - multiple character separator support
--FILE--
<?php
$values = array(1234.5678,
                -1234.5678,
                1234.6578e4,
                -1234.56789e4,
                0x1234CDEF,
                02777777777,
                "123456789",
                "123.456789",
                "12.3456789e1",
                true,
                false);

echo " number_format tests.....multiple character decimal point\n";
for ($i = 0; $i < count($values); $i++) {
    $res = number_format($values[$i], 2, '&#183;', ' ');
    var_dump($res);
}

echo "\n number_format tests.....multiple character thousand separator\n";
for ($i = 0; $i < count($values); $i++) {
    $res = number_format($values[$i], 2, '.' , '&thinsp;');
    var_dump($res);
}

echo "\n number_format tests.....multiple character decimal and thousep\n";
for ($i = 0; $i < count($values); $i++) {
    $res = number_format($values[$i], 2, '&#183;' , '&thinsp;');
    var_dump($res);
}
?>
--EXPECT--
number_format tests.....multiple character decimal point
string(13) "1 234&#183;57"
string(14) "-1 234&#183;57"
string(18) "12 346 578&#183;00"
string(19) "-12 345 678&#183;90"
string(19) "305 450 479&#183;00"
string(19) "402 653 183&#183;00"
string(19) "123 456 789&#183;00"
string(11) "123&#183;46"
string(11) "123&#183;46"
string(9) "1&#183;00"
string(9) "0&#183;00"

 number_format tests.....multiple character thousand separator
string(15) "1&thinsp;234.57"
string(16) "-1&thinsp;234.57"
string(27) "12&thinsp;346&thinsp;578.00"
string(28) "-12&thinsp;345&thinsp;678.90"
string(28) "305&thinsp;450&thinsp;479.00"
string(28) "402&thinsp;653&thinsp;183.00"
string(28) "123&thinsp;456&thinsp;789.00"
string(6) "123.46"
string(6) "123.46"
string(4) "1.00"
string(4) "0.00"

 number_format tests.....multiple character decimal and thousep
string(20) "1&thinsp;234&#183;57"
string(21) "-1&thinsp;234&#183;57"
string(32) "12&thinsp;346&thinsp;578&#183;00"
string(33) "-12&thinsp;345&thinsp;678&#183;90"
string(33) "305&thinsp;450&thinsp;479&#183;00"
string(33) "402&thinsp;653&thinsp;183&#183;00"
string(33) "123&thinsp;456&thinsp;789&#183;00"
string(11) "123&#183;46"
string(11) "123&#183;46"
string(9) "1&#183;00"
string(9) "0&#183;00"
