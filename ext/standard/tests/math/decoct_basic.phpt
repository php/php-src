--TEST--
Test decoct() - basic function test decoct()
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

foreach ($values as $value) {
    try {
       var_dump(decoct($value));
    } catch (TypeError $exception) {
        echo $exception->getMessage() . "\n";
    }
}

?>
--EXPECT--
string(2) "12"
string(4) "7556"
string(4) "7556"
string(1) "3"
string(3) "137"
string(2) "12"
string(4) "7556"
string(4) "7556"
string(2) "47"
decoct(): Argument #1 ($num) must be of type int, string given
string(1) "1"
string(1) "0"
string(1) "0"
