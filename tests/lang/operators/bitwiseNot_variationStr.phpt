--TEST--
Test ~N operator : various numbers as strings
--FILE--
<?php

$strVals = array(
   b"0",b"65",b"-44", b"1.2", b"-7.7", b"abc", b"123abc", b"123e5", b"123e5xyz", b" 123abc", b"123 abc", b"123abc ", b"3.4a",
   b"a5.9"
);


foreach ($strVals as $strVal) {
   echo "--- testing: '$strVal' ---\n";
   var_dump(bin2hex(~$strVal));
}
   
?>
===DONE===
--EXPECT--
--- testing: '0' ---
unicode(2) "cf"
--- testing: '65' ---
unicode(4) "c9ca"
--- testing: '-44' ---
unicode(6) "d2cbcb"
--- testing: '1.2' ---
unicode(6) "ced1cd"
--- testing: '-7.7' ---
unicode(8) "d2c8d1c8"
--- testing: 'abc' ---
unicode(6) "9e9d9c"
--- testing: '123abc' ---
unicode(12) "cecdcc9e9d9c"
--- testing: '123e5' ---
unicode(10) "cecdcc9aca"
--- testing: '123e5xyz' ---
unicode(16) "cecdcc9aca878685"
--- testing: ' 123abc' ---
unicode(14) "dfcecdcc9e9d9c"
--- testing: '123 abc' ---
unicode(14) "cecdccdf9e9d9c"
--- testing: '123abc ' ---
unicode(14) "cecdcc9e9d9cdf"
--- testing: '3.4a' ---
unicode(8) "ccd1cb9e"
--- testing: 'a5.9' ---
unicode(8) "9ecad1c6"
===DONE===
