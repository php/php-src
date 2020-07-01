--TEST--
Using different sorts of numerical strings as a string offset
--FILE--
<?php

$str = "The world is fun";

var_dump($str["7"]);
var_dump($str["7.5"]);
var_dump($str["  7"]);
var_dump($str["  7.5"]);
var_dump($str["  7  "]);
var_dump($str["  7.5  "]);
var_dump($str["7  "]);
var_dump($str["7.5  "]);
var_dump($str["7str"]);
var_dump($str["7.5str"]);
var_dump($str["  7str"]);
var_dump($str["  7.5str"]);
var_dump($str["  7  str"]);
var_dump($str["  7.5  str"]);
var_dump($str["7  str"]);
var_dump($str["7.5  str"]);
var_dump($str["0xC"]);
var_dump($str["0b10"]);
var_dump($str["07"]);

echo "Done\n";
?>
--EXPECTF--
string(1) "l"

Warning: Illegal string offset "7.5" in %s on line 6
string(1) "l"
string(1) "l"

Warning: Illegal string offset "  7.5" in %s on line 8
string(1) "l"
string(1) "l"

Warning: Illegal string offset "  7.5  " in %s on line 10
string(1) "l"
string(1) "l"

Warning: Illegal string offset "7.5  " in %s on line 12
string(1) "l"

Warning: Illegal string offset "7str" in %s on line 13
string(1) "l"

Warning: Illegal string offset "7.5str" in %s on line 14
string(1) "l"

Warning: Illegal string offset "  7str" in %s on line 15
string(1) "l"

Warning: Illegal string offset "  7.5str" in %s on line 16
string(1) "l"

Warning: Illegal string offset "  7  str" in %s on line 17
string(1) "l"

Warning: Illegal string offset "  7.5  str" in %s on line 18
string(1) "l"

Warning: Illegal string offset "7  str" in %s on line 19
string(1) "l"

Warning: Illegal string offset "7.5  str" in %s on line 20
string(1) "l"

Warning: Illegal string offset "0xC" in %s on line 21
string(1) "T"

Warning: Illegal string offset "0b10" in %s on line 22
string(1) "T"
string(1) "l"
Done
