--TEST--
Test % operator : various numbers as strings
--FILE--
<?php

$strVals = array(
   "0","65","-44", "1.2", "-7.7", "abc", "123abc", "123e5", "123e5xyz", " 123abc", "123 abc", "123abc ", "3.4a",
   "a5.9"
);

error_reporting(E_ERROR);

foreach ($strVals as $strVal) {
   foreach($strVals as $otherVal) {
	  echo "--- testing: '$strVal' % '$otherVal' ---\n";
      try {
        var_dump($strVal%$otherVal);
      } catch (DivisionByZeroError $e) {
        echo "Exception: " . $e->getMessage() . "\n";
      }
   }
}

   
?>
===DONE===
--EXPECT--
--- testing: '0' % '0' ---
Exception: Modulo by zero
--- testing: '0' % '65' ---
int(0)
--- testing: '0' % '-44' ---
int(0)
--- testing: '0' % '1.2' ---
int(0)
--- testing: '0' % '-7.7' ---
int(0)
--- testing: '0' % 'abc' ---
Exception: Modulo by zero
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
--- testing: '0' % 'a5.9' ---
Exception: Modulo by zero
--- testing: '65' % '0' ---
Exception: Modulo by zero
--- testing: '65' % '65' ---
int(0)
--- testing: '65' % '-44' ---
int(21)
--- testing: '65' % '1.2' ---
int(0)
--- testing: '65' % '-7.7' ---
int(2)
--- testing: '65' % 'abc' ---
Exception: Modulo by zero
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
--- testing: '65' % 'a5.9' ---
Exception: Modulo by zero
--- testing: '-44' % '0' ---
Exception: Modulo by zero
--- testing: '-44' % '65' ---
int(-44)
--- testing: '-44' % '-44' ---
int(0)
--- testing: '-44' % '1.2' ---
int(0)
--- testing: '-44' % '-7.7' ---
int(-2)
--- testing: '-44' % 'abc' ---
Exception: Modulo by zero
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
--- testing: '-44' % 'a5.9' ---
Exception: Modulo by zero
--- testing: '1.2' % '0' ---
Exception: Modulo by zero
--- testing: '1.2' % '65' ---
int(1)
--- testing: '1.2' % '-44' ---
int(1)
--- testing: '1.2' % '1.2' ---
int(0)
--- testing: '1.2' % '-7.7' ---
int(1)
--- testing: '1.2' % 'abc' ---
Exception: Modulo by zero
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
--- testing: '1.2' % 'a5.9' ---
Exception: Modulo by zero
--- testing: '-7.7' % '0' ---
Exception: Modulo by zero
--- testing: '-7.7' % '65' ---
int(-7)
--- testing: '-7.7' % '-44' ---
int(-7)
--- testing: '-7.7' % '1.2' ---
int(0)
--- testing: '-7.7' % '-7.7' ---
int(0)
--- testing: '-7.7' % 'abc' ---
Exception: Modulo by zero
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
--- testing: '-7.7' % 'a5.9' ---
Exception: Modulo by zero
--- testing: 'abc' % '0' ---
Exception: Modulo by zero
--- testing: 'abc' % '65' ---
int(0)
--- testing: 'abc' % '-44' ---
int(0)
--- testing: 'abc' % '1.2' ---
int(0)
--- testing: 'abc' % '-7.7' ---
int(0)
--- testing: 'abc' % 'abc' ---
Exception: Modulo by zero
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
--- testing: 'abc' % 'a5.9' ---
Exception: Modulo by zero
--- testing: '123abc' % '0' ---
Exception: Modulo by zero
--- testing: '123abc' % '65' ---
int(58)
--- testing: '123abc' % '-44' ---
int(35)
--- testing: '123abc' % '1.2' ---
int(0)
--- testing: '123abc' % '-7.7' ---
int(4)
--- testing: '123abc' % 'abc' ---
Exception: Modulo by zero
--- testing: '123abc' % '123abc' ---
int(0)
--- testing: '123abc' % '123e5' ---
int(123)
--- testing: '123abc' % '123e5xyz' ---
int(123)
--- testing: '123abc' % ' 123abc' ---
int(0)
--- testing: '123abc' % '123 abc' ---
int(0)
--- testing: '123abc' % '123abc ' ---
int(0)
--- testing: '123abc' % '3.4a' ---
int(0)
--- testing: '123abc' % 'a5.9' ---
Exception: Modulo by zero
--- testing: '123e5' % '0' ---
Exception: Modulo by zero
--- testing: '123e5' % '65' ---
int(50)
--- testing: '123e5' % '-44' ---
int(20)
--- testing: '123e5' % '1.2' ---
int(0)
--- testing: '123e5' % '-7.7' ---
int(6)
--- testing: '123e5' % 'abc' ---
Exception: Modulo by zero
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
--- testing: '123e5' % 'a5.9' ---
Exception: Modulo by zero
--- testing: '123e5xyz' % '0' ---
Exception: Modulo by zero
--- testing: '123e5xyz' % '65' ---
int(50)
--- testing: '123e5xyz' % '-44' ---
int(20)
--- testing: '123e5xyz' % '1.2' ---
int(0)
--- testing: '123e5xyz' % '-7.7' ---
int(6)
--- testing: '123e5xyz' % 'abc' ---
Exception: Modulo by zero
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
--- testing: '123e5xyz' % 'a5.9' ---
Exception: Modulo by zero
--- testing: ' 123abc' % '0' ---
Exception: Modulo by zero
--- testing: ' 123abc' % '65' ---
int(58)
--- testing: ' 123abc' % '-44' ---
int(35)
--- testing: ' 123abc' % '1.2' ---
int(0)
--- testing: ' 123abc' % '-7.7' ---
int(4)
--- testing: ' 123abc' % 'abc' ---
Exception: Modulo by zero
--- testing: ' 123abc' % '123abc' ---
int(0)
--- testing: ' 123abc' % '123e5' ---
int(123)
--- testing: ' 123abc' % '123e5xyz' ---
int(123)
--- testing: ' 123abc' % ' 123abc' ---
int(0)
--- testing: ' 123abc' % '123 abc' ---
int(0)
--- testing: ' 123abc' % '123abc ' ---
int(0)
--- testing: ' 123abc' % '3.4a' ---
int(0)
--- testing: ' 123abc' % 'a5.9' ---
Exception: Modulo by zero
--- testing: '123 abc' % '0' ---
Exception: Modulo by zero
--- testing: '123 abc' % '65' ---
int(58)
--- testing: '123 abc' % '-44' ---
int(35)
--- testing: '123 abc' % '1.2' ---
int(0)
--- testing: '123 abc' % '-7.7' ---
int(4)
--- testing: '123 abc' % 'abc' ---
Exception: Modulo by zero
--- testing: '123 abc' % '123abc' ---
int(0)
--- testing: '123 abc' % '123e5' ---
int(123)
--- testing: '123 abc' % '123e5xyz' ---
int(123)
--- testing: '123 abc' % ' 123abc' ---
int(0)
--- testing: '123 abc' % '123 abc' ---
int(0)
--- testing: '123 abc' % '123abc ' ---
int(0)
--- testing: '123 abc' % '3.4a' ---
int(0)
--- testing: '123 abc' % 'a5.9' ---
Exception: Modulo by zero
--- testing: '123abc ' % '0' ---
Exception: Modulo by zero
--- testing: '123abc ' % '65' ---
int(58)
--- testing: '123abc ' % '-44' ---
int(35)
--- testing: '123abc ' % '1.2' ---
int(0)
--- testing: '123abc ' % '-7.7' ---
int(4)
--- testing: '123abc ' % 'abc' ---
Exception: Modulo by zero
--- testing: '123abc ' % '123abc' ---
int(0)
--- testing: '123abc ' % '123e5' ---
int(123)
--- testing: '123abc ' % '123e5xyz' ---
int(123)
--- testing: '123abc ' % ' 123abc' ---
int(0)
--- testing: '123abc ' % '123 abc' ---
int(0)
--- testing: '123abc ' % '123abc ' ---
int(0)
--- testing: '123abc ' % '3.4a' ---
int(0)
--- testing: '123abc ' % 'a5.9' ---
Exception: Modulo by zero
--- testing: '3.4a' % '0' ---
Exception: Modulo by zero
--- testing: '3.4a' % '65' ---
int(3)
--- testing: '3.4a' % '-44' ---
int(3)
--- testing: '3.4a' % '1.2' ---
int(0)
--- testing: '3.4a' % '-7.7' ---
int(3)
--- testing: '3.4a' % 'abc' ---
Exception: Modulo by zero
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
--- testing: '3.4a' % 'a5.9' ---
Exception: Modulo by zero
--- testing: 'a5.9' % '0' ---
Exception: Modulo by zero
--- testing: 'a5.9' % '65' ---
int(0)
--- testing: 'a5.9' % '-44' ---
int(0)
--- testing: 'a5.9' % '1.2' ---
int(0)
--- testing: 'a5.9' % '-7.7' ---
int(0)
--- testing: 'a5.9' % 'abc' ---
Exception: Modulo by zero
--- testing: 'a5.9' % '123abc' ---
int(0)
--- testing: 'a5.9' % '123e5' ---
int(0)
--- testing: 'a5.9' % '123e5xyz' ---
int(0)
--- testing: 'a5.9' % ' 123abc' ---
int(0)
--- testing: 'a5.9' % '123 abc' ---
int(0)
--- testing: 'a5.9' % '123abc ' ---
int(0)
--- testing: 'a5.9' % '3.4a' ---
int(0)
--- testing: 'a5.9' % 'a5.9' ---
Exception: Modulo by zero
===DONE===
