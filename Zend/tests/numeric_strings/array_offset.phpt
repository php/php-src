--TEST--
Using different sorts of numerical strings as an array offset
--FILE--
<?php

$arr = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15];

var_dump($arr["7"]);
var_dump($arr["7.5"]);
var_dump($arr["  7"]);
var_dump($arr["  7.5"]);
var_dump($arr["  7  "]);
var_dump($arr["  7.5  "]);
var_dump($arr["7  "]);
var_dump($arr["7.5  "]);
var_dump($arr["7str"]);
var_dump($arr["7.5str"]);
var_dump($arr["  7str"]);
var_dump($arr["  7.5str"]);
var_dump($arr["  7  str"]);
var_dump($arr["  7.5  str"]);
var_dump($arr["7  str"]);
var_dump($arr["7.5  str"]);
var_dump($arr["0xA"]);
var_dump($arr["0b10"]);
var_dump($arr["07"]);

echo "Done\n";
?>
--EXPECTF--
int(7)

Warning: Undefined array key "7.5" in %s on line %d
NULL

Warning: Undefined array key "  7" in %s on line %d
NULL

Warning: Undefined array key "  7.5" in %s on line %d
NULL

Warning: Undefined array key "  7  " in %s on line %d
NULL

Warning: Undefined array key "  7.5  " in %s on line %d
NULL

Warning: Undefined array key "7  " in %s on line %d
NULL

Warning: Undefined array key "7.5  " in %s on line %d
NULL

Warning: Undefined array key "7str" in %s on line %d
NULL

Warning: Undefined array key "7.5str" in %s on line %d
NULL

Warning: Undefined array key "  7str" in %s on line %d
NULL

Warning: Undefined array key "  7.5str" in %s on line %d
NULL

Warning: Undefined array key "  7  str" in %s on line %d
NULL

Warning: Undefined array key "  7.5  str" in %s on line %d
NULL

Warning: Undefined array key "7  str" in %s on line %d
NULL

Warning: Undefined array key "7.5  str" in %s on line %d
NULL

Warning: Undefined array key "0xA" in %s on line %d
NULL

Warning: Undefined array key "0b10" in %s on line %d
NULL

Warning: Undefined array key "07" in %s on line %d
NULL
Done
