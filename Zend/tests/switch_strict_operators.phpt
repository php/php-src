--TEST--
Switch with strict_operators
--FILE--
<?php
declare(strict_operators=1);

function test($value) {
    switch ($value) {
        case 0: return 0;
        case 1: return 1;

        case "01": return "01";
        case "1": return "1";

        case " 2": return " 2";
        case "2": return "2";

        case 10.0: return 10.0;
        case "10.0": return "10.0";
        case "1e1": return "1e1";

        default: return "default";
    }
}

var_dump(test(1));
var_dump(test("1"));
var_dump(test("2"));
var_dump(test("10.0"));
var_dump(test("1e1"));
var_dump(test(null));

?>
--EXPECT--
int(1)
string(1) "1"
string(1) "2"
string(4) "10.0"
string(3) "1e1"
string(7) "default"