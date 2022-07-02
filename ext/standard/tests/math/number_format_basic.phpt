--TEST--
Test number_format() - basic function test number_format()
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

echo "\n number_format tests.....default\n";
for ($i = 0; $i < count($values); $i++) {
    $res = number_format($values[$i]);
    var_dump($res);
}

echo "\n number_format tests.....with two dp\n";
for ($i = 0; $i < count($values); $i++) {
    $res = number_format($values[$i], 2);
    var_dump($res);
}

echo "\n number_format tests.....English format\n";
for ($i = 0; $i < count($values); $i++) {
    $res = number_format($values[$i], 2, '.', ' ');
    var_dump($res);
}

echo "\n number_format tests.....French format\n";
for ($i = 0; $i < count($values); $i++) {
    $res = number_format($values[$i], 2, ',' , ' ');
    var_dump($res);
}
?>
--EXPECT--
number_format tests.....default
string(5) "1,235"
string(6) "-1,235"
string(10) "12,346,578"
string(11) "-12,345,679"
string(11) "305,450,479"
string(11) "402,653,183"
string(11) "123,456,789"
string(3) "123"
string(3) "123"
string(1) "1"
string(1) "0"

 number_format tests.....with two dp
string(8) "1,234.57"
string(9) "-1,234.57"
string(13) "12,346,578.00"
string(14) "-12,345,678.90"
string(14) "305,450,479.00"
string(14) "402,653,183.00"
string(14) "123,456,789.00"
string(6) "123.46"
string(6) "123.46"
string(4) "1.00"
string(4) "0.00"

 number_format tests.....English format
string(8) "1 234.57"
string(9) "-1 234.57"
string(13) "12 346 578.00"
string(14) "-12 345 678.90"
string(14) "305 450 479.00"
string(14) "402 653 183.00"
string(14) "123 456 789.00"
string(6) "123.46"
string(6) "123.46"
string(4) "1.00"
string(4) "0.00"

 number_format tests.....French format
string(8) "1 234,57"
string(9) "-1 234,57"
string(13) "12 346 578,00"
string(14) "-12 345 678,90"
string(14) "305 450 479,00"
string(14) "402 653 183,00"
string(14) "123 456 789,00"
string(6) "123,46"
string(6) "123,46"
string(4) "1,00"
string(4) "0,00"
