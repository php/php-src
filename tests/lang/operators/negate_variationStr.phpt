--TEST--
Test -N operator : various numbers as strings
--FILE--
<?php

$strVals = array(
   "0","65","-44", "1.2", "-7.7", "abc", "123abc", "123e5", "123e5xyz", " 123abc", "123 abc", "123abc ", "3.4a",
   "a5.9"
);


foreach ($strVals as $strVal) {
   echo "--- testing: '$strVal' ---\n";
   var_dump(-$strVal);
}

?>
===DONE===
--EXPECTF--
--- testing: '0' ---
int(0)
--- testing: '65' ---
int(-65)
--- testing: '-44' ---
int(44)
--- testing: '1.2' ---
float(-1.2)
--- testing: '-7.7' ---
float(7.7)
--- testing: 'abc' ---

Warning: A non-numeric value encountered in %s on line %d
int(0)
--- testing: '123abc' ---

Notice: A non well formed numeric value encountered in %s on line %d
int(-123)
--- testing: '123e5' ---
float(-12300000)
--- testing: '123e5xyz' ---

Notice: A non well formed numeric value encountered in %s on line %d
float(-12300000)
--- testing: ' 123abc' ---

Notice: A non well formed numeric value encountered in %s on line %d
int(-123)
--- testing: '123 abc' ---

Notice: A non well formed numeric value encountered in %s on line %d
int(-123)
--- testing: '123abc ' ---

Notice: A non well formed numeric value encountered in %s on line %d
int(-123)
--- testing: '3.4a' ---

Notice: A non well formed numeric value encountered in %s on line %d
float(-3.4)
--- testing: 'a5.9' ---

Warning: A non-numeric value encountered in %s on line %d
int(0)
===DONE===
