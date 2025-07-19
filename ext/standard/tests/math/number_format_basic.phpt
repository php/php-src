--TEST--
Test number_format() - basic function test number_format()
--FILE--
<?php
$values = [
    '1234.5678',
    '-1234.5678',
    '1234.6578e4',
    '1234.56789e4',
    '999999',
    '-999999',
    '999999.0',
    '-999999.0',
];

echo "number_format tests.....default\n";
foreach ($values as $value) {
    $res = number_format($value);
    var_dump($res);
}

echo "number_format tests.....with two dp\n";
foreach ($values as $value) {
    $res = number_format($value, 2);
    var_dump($res);
}

echo "number_format tests.....English format\n";
foreach ($values as $value) {
    $res = number_format($value, 2, '.', ' ');
    var_dump($res);
}

echo "number_format tests.....French format\n";
foreach ($values as $value) {
    $res = number_format($value, 2, ',' , ' ');
    var_dump($res);
}

echo "number_format tests.....multichar format\n";
foreach ($values as $value) {
    $res = number_format($value, 2, ' DECIMALS ' , ' THOUSAND ');
    var_dump($res);
}
?>
--EXPECT--
number_format tests.....default
string(5) "1,235"
string(6) "-1,235"
string(10) "12,346,578"
string(10) "12,345,679"
string(7) "999,999"
string(8) "-999,999"
string(7) "999,999"
string(8) "-999,999"
number_format tests.....with two dp
string(8) "1,234.57"
string(9) "-1,234.57"
string(13) "12,346,578.00"
string(13) "12,345,678.90"
string(10) "999,999.00"
string(11) "-999,999.00"
string(10) "999,999.00"
string(11) "-999,999.00"
number_format tests.....English format
string(8) "1 234.57"
string(9) "-1 234.57"
string(13) "12 346 578.00"
string(13) "12 345 678.90"
string(10) "999 999.00"
string(11) "-999 999.00"
string(10) "999 999.00"
string(11) "-999 999.00"
number_format tests.....French format
string(8) "1 234,57"
string(9) "-1 234,57"
string(13) "12 346 578,00"
string(13) "12 345 678,90"
string(10) "999 999,00"
string(11) "-999 999,00"
string(10) "999 999,00"
string(11) "-999 999,00"
number_format tests.....multichar format
string(26) "1 THOUSAND 234 DECIMALS 57"
string(27) "-1 THOUSAND 234 DECIMALS 57"
string(40) "12 THOUSAND 346 THOUSAND 578 DECIMALS 00"
string(40) "12 THOUSAND 345 THOUSAND 678 DECIMALS 90"
string(28) "999 THOUSAND 999 DECIMALS 00"
string(29) "-999 THOUSAND 999 DECIMALS 00"
string(28) "999 THOUSAND 999 DECIMALS 00"
string(29) "-999 THOUSAND 999 DECIMALS 00"
