--TEST--
Switch on numeric strings
--FILE--
<?php

function test($value) {
    switch ($value) {
        case "01": return "01";
        case "1": return "1";

        case " 2": return " 2";
        case "2": return "2";

        case "10.0": return "10.0";
        case "1e1": return "1e1";

        default: return "default";
    }
}

var_dump(test("1"));
var_dump(test("2"));
var_dump(test("1e1"));

?>
--EXPECT--
string(2) "01"
string(2) " 2"
string(4) "10.0"
