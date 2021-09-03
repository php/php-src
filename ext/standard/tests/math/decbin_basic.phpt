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
                );

foreach ($values as $value) {
    try {
       var_dump(decbin($value));
    } catch (TypeError $exception) {
        echo $exception->getMessage() . "\n";
    }
}

?>
--EXPECTF--
string(4) "1010"

Deprecated: Implicit conversion from float 3950.5 to int loses precision in %s on line %d
string(12) "111101101110"

Deprecated: Implicit conversion from float 3950.5 to int loses precision in %s on line %d
string(12) "111101101110"
string(2) "11"
string(7) "1011111"
string(4) "1010"

Deprecated: Implicit conversion from float-string "3950.5" to int loses precision in %s on line %d
string(12) "111101101110"

Deprecated: Implicit conversion from float-string "3.9505e3" to int loses precision in %s on line %d
string(12) "111101101110"
string(6) "100111"
decbin(): Argument #1 ($num) must be of type int, string given
string(1) "1"
string(1) "0"
