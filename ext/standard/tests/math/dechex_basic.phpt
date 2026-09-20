--TEST--
Test dechex() - basic function dechex()
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
                true,
                false,
                );

foreach ($values as $value) {
    var_dump(dechex($value));
}

?>
--EXPECTF--
string(1) "a"

Deprecated: Implicit conversion from float 3950.5 to int loses precision in %s on line %d
string(3) "f6e"

Deprecated: Implicit conversion from float 3950.5 to int loses precision in %s on line %d
string(3) "f6e"
string(1) "3"
string(2) "5f"
string(1) "a"

Deprecated: Implicit conversion from float-string "3950.5" to int loses precision in %s on line %d
string(3) "f6e"

Deprecated: Implicit conversion from float-string "3.9505e3" to int loses precision in %s on line %d
string(3) "f6e"
string(2) "27"
string(1) "1"
string(1) "0"
