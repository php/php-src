--TEST--
Test decbin() - basic function test
--FILE--
<?php
$values = array(10,
                3950.5,
                3.9505e3,
                03,
                0x5F,
                "10",
                "3950.5",
                "3.9505e3",
                "039",
                "0x5F",
                true,
                false,
                null,
                );

for ($i = 0; $i < count($values); $i++) {
    $res = decbin($values[$i]);
    var_dump($res);
}
?>
--EXPECT--
string(4) "1010"
string(12) "111101101110"
string(12) "111101101110"
string(2) "11"
string(7) "1011111"
string(4) "1010"
string(12) "111101101110"
string(12) "111101101110"
string(6) "100111"
string(1) "0"
string(1) "1"
string(1) "0"
string(1) "0"
