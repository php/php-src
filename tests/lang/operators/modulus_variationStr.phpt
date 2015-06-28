--TEST--
Test % operator : various numbers as strings
--FILE--
<?php

$strVals = ["65", "-44", "1.2", "-7.7", "123abc", "123e5", "123e5xyz", " 123abc", "123 abc", "123abc ", "3.4a"];

error_reporting(E_ERROR);

foreach (array_merge(["0", "abc"], $strVals) as $strVal) {
   foreach($strVals as $otherVal) {
	  echo "--- testing: '$strVal' % '$otherVal' ---\n";
      try {
        var_dump($strVal%$otherVal);
      } catch (Exception $e) {
        echo "Exception: " . $e->getMessage() . "\n";
      }
   }
}

   
?>
===DONE===
--EXPECT--
--- testing: '0' % '65' ---
int(0)
--- testing: '0' % '-44' ---
int(0)
--- testing: '0' % '1.2' ---
int(0)
--- testing: '0' % '-7.7' ---
int(0)
--- testing: '0' % '123abc' ---
int(0)
--- testing: '0' % '123e5' ---
int(0)
--- testing: '0' % '123e5xyz' ---
int(0)
--- testing: '0' % ' 123abc' ---
int(0)
--- testing: '0' % '123 abc' ---
int(0)
--- testing: '0' % '123abc ' ---
int(0)
--- testing: '0' % '3.4a' ---
int(0)
--- testing: 'abc' % '65' ---
int(0)
--- testing: 'abc' % '-44' ---
int(0)
--- testing: 'abc' % '1.2' ---
int(0)
--- testing: 'abc' % '-7.7' ---
int(0)
--- testing: 'abc' % '123abc' ---
int(0)
--- testing: 'abc' % '123e5' ---
int(0)
--- testing: 'abc' % '123e5xyz' ---
int(0)
--- testing: 'abc' % ' 123abc' ---
int(0)
--- testing: 'abc' % '123 abc' ---
int(0)
--- testing: 'abc' % '123abc ' ---
int(0)
--- testing: 'abc' % '3.4a' ---
int(0)
--- testing: '65' % '65' ---
int(0)
--- testing: '65' % '-44' ---
int(21)
--- testing: '65' % '1.2' ---
int(0)
--- testing: '65' % '-7.7' ---
int(2)
--- testing: '65' % '123abc' ---
int(65)
--- testing: '65' % '123e5' ---
int(65)
--- testing: '65' % '123e5xyz' ---
int(65)
--- testing: '65' % ' 123abc' ---
int(65)
--- testing: '65' % '123 abc' ---
int(65)
--- testing: '65' % '123abc ' ---
int(65)
--- testing: '65' % '3.4a' ---
int(2)
--- testing: '-44' % '65' ---
int(-44)
--- testing: '-44' % '-44' ---
int(0)
--- testing: '-44' % '1.2' ---
int(0)
--- testing: '-44' % '-7.7' ---
int(-2)
--- testing: '-44' % '123abc' ---
int(-44)
--- testing: '-44' % '123e5' ---
int(-44)
--- testing: '-44' % '123e5xyz' ---
int(-44)
--- testing: '-44' % ' 123abc' ---
int(-44)
--- testing: '-44' % '123 abc' ---
int(-44)
--- testing: '-44' % '123abc ' ---
int(-44)
--- testing: '-44' % '3.4a' ---
int(-2)
--- testing: '1.2' % '65' ---
int(1)
--- testing: '1.2' % '-44' ---
int(1)
--- testing: '1.2' % '1.2' ---
int(0)
--- testing: '1.2' % '-7.7' ---
int(1)
--- testing: '1.2' % '123abc' ---
int(1)
--- testing: '1.2' % '123e5' ---
int(1)
--- testing: '1.2' % '123e5xyz' ---
int(1)
--- testing: '1.2' % ' 123abc' ---
int(1)
--- testing: '1.2' % '123 abc' ---
int(1)
--- testing: '1.2' % '123abc ' ---
int(1)
--- testing: '1.2' % '3.4a' ---
int(1)
--- testing: '-7.7' % '65' ---
int(-7)
--- testing: '-7.7' % '-44' ---
int(-7)
--- testing: '-7.7' % '1.2' ---
int(0)
--- testing: '-7.7' % '-7.7' ---
int(0)
--- testing: '-7.7' % '123abc' ---
int(-7)
--- testing: '-7.7' % '123e5' ---
int(-7)
--- testing: '-7.7' % '123e5xyz' ---
int(-7)
--- testing: '-7.7' % ' 123abc' ---
int(-7)
--- testing: '-7.7' % '123 abc' ---
int(-7)
--- testing: '-7.7' % '123abc ' ---
int(-7)
--- testing: '-7.7' % '3.4a' ---
int(-1)
--- testing: '123abc' % '65' ---
int(58)
--- testing: '123abc' % '-44' ---
int(35)
--- testing: '123abc' % '1.2' ---
int(0)
--- testing: '123abc' % '-7.7' ---
int(4)
--- testing: '123abc' % '123abc' ---
int(0)
--- testing: '123abc' % '123e5' ---
int(0)
--- testing: '123abc' % '123e5xyz' ---
int(0)
--- testing: '123abc' % ' 123abc' ---
int(0)
--- testing: '123abc' % '123 abc' ---
int(0)
--- testing: '123abc' % '123abc ' ---
int(0)
--- testing: '123abc' % '3.4a' ---
int(0)
--- testing: '123e5' % '65' ---
int(58)
--- testing: '123e5' % '-44' ---
int(35)
--- testing: '123e5' % '1.2' ---
int(0)
--- testing: '123e5' % '-7.7' ---
int(4)
--- testing: '123e5' % '123abc' ---
int(0)
--- testing: '123e5' % '123e5' ---
int(0)
--- testing: '123e5' % '123e5xyz' ---
int(0)
--- testing: '123e5' % ' 123abc' ---
int(0)
--- testing: '123e5' % '123 abc' ---
int(0)
--- testing: '123e5' % '123abc ' ---
int(0)
--- testing: '123e5' % '3.4a' ---
int(0)
--- testing: '123e5xyz' % '65' ---
int(58)
--- testing: '123e5xyz' % '-44' ---
int(35)
--- testing: '123e5xyz' % '1.2' ---
int(0)
--- testing: '123e5xyz' % '-7.7' ---
int(4)
--- testing: '123e5xyz' % '123abc' ---
int(0)
--- testing: '123e5xyz' % '123e5' ---
int(0)
--- testing: '123e5xyz' % '123e5xyz' ---
int(0)
--- testing: '123e5xyz' % ' 123abc' ---
int(0)
--- testing: '123e5xyz' % '123 abc' ---
int(0)
--- testing: '123e5xyz' % '123abc ' ---
int(0)
--- testing: '123e5xyz' % '3.4a' ---
int(0)
--- testing: ' 123abc' % '65' ---
int(58)
--- testing: ' 123abc' % '-44' ---
int(35)
--- testing: ' 123abc' % '1.2' ---
int(0)
--- testing: ' 123abc' % '-7.7' ---
int(4)
--- testing: ' 123abc' % '123abc' ---
int(0)
--- testing: ' 123abc' % '123e5' ---
int(0)
--- testing: ' 123abc' % '123e5xyz' ---
int(0)
--- testing: ' 123abc' % ' 123abc' ---
int(0)
--- testing: ' 123abc' % '123 abc' ---
int(0)
--- testing: ' 123abc' % '123abc ' ---
int(0)
--- testing: ' 123abc' % '3.4a' ---
int(0)
--- testing: '123 abc' % '65' ---
int(58)
--- testing: '123 abc' % '-44' ---
int(35)
--- testing: '123 abc' % '1.2' ---
int(0)
--- testing: '123 abc' % '-7.7' ---
int(4)
--- testing: '123 abc' % '123abc' ---
int(0)
--- testing: '123 abc' % '123e5' ---
int(0)
--- testing: '123 abc' % '123e5xyz' ---
int(0)
--- testing: '123 abc' % ' 123abc' ---
int(0)
--- testing: '123 abc' % '123 abc' ---
int(0)
--- testing: '123 abc' % '123abc ' ---
int(0)
--- testing: '123 abc' % '3.4a' ---
int(0)
--- testing: '123abc ' % '65' ---
int(58)
--- testing: '123abc ' % '-44' ---
int(35)
--- testing: '123abc ' % '1.2' ---
int(0)
--- testing: '123abc ' % '-7.7' ---
int(4)
--- testing: '123abc ' % '123abc' ---
int(0)
--- testing: '123abc ' % '123e5' ---
int(0)
--- testing: '123abc ' % '123e5xyz' ---
int(0)
--- testing: '123abc ' % ' 123abc' ---
int(0)
--- testing: '123abc ' % '123 abc' ---
int(0)
--- testing: '123abc ' % '123abc ' ---
int(0)
--- testing: '123abc ' % '3.4a' ---
int(0)
--- testing: '3.4a' % '65' ---
int(3)
--- testing: '3.4a' % '-44' ---
int(3)
--- testing: '3.4a' % '1.2' ---
int(0)
--- testing: '3.4a' % '-7.7' ---
int(3)
--- testing: '3.4a' % '123abc' ---
int(3)
--- testing: '3.4a' % '123e5' ---
int(3)
--- testing: '3.4a' % '123e5xyz' ---
int(3)
--- testing: '3.4a' % ' 123abc' ---
int(3)
--- testing: '3.4a' % '123 abc' ---
int(3)
--- testing: '3.4a' % '123abc ' ---
int(3)
--- testing: '3.4a' % '3.4a' ---
int(0)
===DONE===
