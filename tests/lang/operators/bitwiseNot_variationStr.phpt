--TEST--
Test ~N operator : various numbers as strings
--FILE--
<?php

$strVals = array(
   "0","65","-44", "1.2", "-7.7", "abc", "123abc", "123e5", "123e5xyz", " 123abc", "123 abc", "123abc ", "3.4a",
   "a5.9"
);


foreach ($strVals as $strVal) {
   echo "--- testing: '$strVal' ---\n";
   var_dump(bin2hex(~$strVal));
}
   
?>
===DONE===
--EXPECT--
--- testing: '0' ---
string(2) "cf"
--- testing: '65' ---
string(4) "c9ca"
--- testing: '-44' ---
string(6) "d2cbcb"
--- testing: '1.2' ---
string(6) "ced1cd"
--- testing: '-7.7' ---
string(8) "d2c8d1c8"
--- testing: 'abc' ---
string(6) "9e9d9c"
--- testing: '123abc' ---
string(12) "cecdcc9e9d9c"
--- testing: '123e5' ---
string(10) "cecdcc9aca"
--- testing: '123e5xyz' ---
string(16) "cecdcc9aca878685"
--- testing: ' 123abc' ---
string(14) "dfcecdcc9e9d9c"
--- testing: '123 abc' ---
string(14) "cecdccdf9e9d9c"
--- testing: '123abc ' ---
string(14) "cecdcc9e9d9cdf"
--- testing: '3.4a' ---
string(8) "ccd1cb9e"
--- testing: 'a5.9' ---
string(8) "9ecad1c6"
===DONE===
