--TEST--
Test ++N operator : various numbers as strings
--FILE--
<?php

$strVals = array(
    "0","65","-44", "1.2", "-7.7",
    "abc", "123abc", "123e5", "123e5xyz", " 123abc", "123 abc", "123abc ", "3.4a", "a5.9",
    "z", "az", "zz", "Z", "AZ", "ZZ", "9z", "19z", "99z",
);


foreach ($strVals as $strVal) {
    echo "--- testing: '$strVal' ---\n";
    var_dump(++$strVal);
}

?>
--EXPECTF--
--- testing: '0' ---
int(1)
--- testing: '65' ---
int(66)
--- testing: '-44' ---
int(-43)
--- testing: '1.2' ---
float(2.2)
--- testing: '-7.7' ---
float(-6.7)
--- testing: 'abc' ---

Deprecated: Increment on non-numeric string is deprecated, use str_increment() instead in %s on line %d
string(3) "abd"
--- testing: '123abc' ---

Deprecated: Increment on non-numeric string is deprecated, use str_increment() instead in %s on line %d
string(6) "123abd"
--- testing: '123e5' ---
float(12300001)
--- testing: '123e5xyz' ---

Deprecated: Increment on non-numeric string is deprecated, use str_increment() instead in %s on line %d
string(8) "123e5xza"
--- testing: ' 123abc' ---

Deprecated: Increment on non-numeric string is deprecated, use str_increment() instead in %s on line %d
string(7) " 123abd"
--- testing: '123 abc' ---

Deprecated: Increment on non-numeric string is deprecated, use str_increment() instead in %s on line %d
string(7) "123 abd"
--- testing: '123abc ' ---

Deprecated: Increment on non-numeric string is deprecated, use str_increment() instead in %s on line %d
string(7) "123abc "
--- testing: '3.4a' ---

Deprecated: Increment on non-numeric string is deprecated, use str_increment() instead in %s on line %d
string(4) "3.4b"
--- testing: 'a5.9' ---

Deprecated: Increment on non-numeric string is deprecated, use str_increment() instead in %s on line %d
string(4) "a5.0"
--- testing: 'z' ---

Deprecated: Increment on non-numeric string is deprecated, use str_increment() instead in %s on line %d
string(2) "aa"
--- testing: 'az' ---

Deprecated: Increment on non-numeric string is deprecated, use str_increment() instead in %s on line %d
string(2) "ba"
--- testing: 'zz' ---

Deprecated: Increment on non-numeric string is deprecated, use str_increment() instead in %s on line %d
string(3) "aaa"
--- testing: 'Z' ---

Deprecated: Increment on non-numeric string is deprecated, use str_increment() instead in %s on line %d
string(2) "AA"
--- testing: 'AZ' ---

Deprecated: Increment on non-numeric string is deprecated, use str_increment() instead in %s on line %d
string(2) "BA"
--- testing: 'ZZ' ---

Deprecated: Increment on non-numeric string is deprecated, use str_increment() instead in %s on line %d
string(3) "AAA"
--- testing: '9z' ---

Deprecated: Increment on non-numeric string is deprecated, use str_increment() instead in %s on line %d
string(3) "10a"
--- testing: '19z' ---

Deprecated: Increment on non-numeric string is deprecated, use str_increment() instead in %s on line %d
string(3) "20a"
--- testing: '99z' ---

Deprecated: Increment on non-numeric string is deprecated, use str_increment() instead in %s on line %d
string(4) "100a"
