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

Notice: Undefined array key "7.5" in %s on line 6
NULL

Notice: Undefined array key "  7" in %s on line 7
NULL

Notice: Undefined array key "  7.5" in %s on line 8
NULL

Notice: Undefined array key "  7  " in %s on line 9
NULL

Notice: Undefined array key "  7.5  " in %s on line 10
NULL

Notice: Undefined array key "7  " in %s on line 11
NULL

Notice: Undefined array key "7.5  " in %s on line 12
NULL

Notice: Undefined array key "7str" in %s on line 13
NULL

Notice: Undefined array key "7.5str" in %s on line 14
NULL

Notice: Undefined array key "  7str" in %s on line 15
NULL

Notice: Undefined array key "  7.5str" in %s on line 16
NULL

Notice: Undefined array key "  7  str" in %s on line 17
NULL

Notice: Undefined array key "  7.5  str" in %s on line 18
NULL

Notice: Undefined array key "7  str" in %s on line 19
NULL

Notice: Undefined array key "7.5  str" in %s on line 20
NULL

Notice: Undefined array key "0xA" in %s on line 21
NULL

Notice: Undefined array key "0b10" in %s on line 22
NULL

Notice: Undefined array key "07" in %s on line 23
NULL
Done
