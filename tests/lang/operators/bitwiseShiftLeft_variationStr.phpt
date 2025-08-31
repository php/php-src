--TEST--
Test << operator : various numbers as strings
--FILE--
<?php

$strVals = array(
   "0","65","-44", "1.2", "-7.7", "abc", "123abc", "123e5", "123e5xyz", " 123abc", "123 abc", "123abc ", "3.4a",
   "a5.9"
);

error_reporting(E_ERROR);

foreach ($strVals as $strVal) {
   foreach($strVals as $otherVal) {
      echo "--- testing: '$strVal' << '$otherVal' ---\n";
      try {
        var_dump($strVal<<$otherVal);
      } catch (Throwable $e) {
        echo get_class($e) . ': ' . $e->getMessage() . "\n";
      }
   }
}

?>
--EXPECT--
--- testing: '0' << '0' ---
int(0)
--- testing: '0' << '65' ---
int(0)
--- testing: '0' << '-44' ---
ArithmeticError: Bit shift by negative number
--- testing: '0' << '1.2' ---
int(0)
--- testing: '0' << '-7.7' ---
ArithmeticError: Bit shift by negative number
--- testing: '0' << 'abc' ---
TypeError: Unsupported operand types: string << string
--- testing: '0' << '123abc' ---
int(0)
--- testing: '0' << '123e5' ---
int(0)
--- testing: '0' << '123e5xyz' ---
int(0)
--- testing: '0' << ' 123abc' ---
int(0)
--- testing: '0' << '123 abc' ---
int(0)
--- testing: '0' << '123abc ' ---
int(0)
--- testing: '0' << '3.4a' ---
int(0)
--- testing: '0' << 'a5.9' ---
TypeError: Unsupported operand types: string << string
--- testing: '65' << '0' ---
int(65)
--- testing: '65' << '65' ---
int(0)
--- testing: '65' << '-44' ---
ArithmeticError: Bit shift by negative number
--- testing: '65' << '1.2' ---
int(130)
--- testing: '65' << '-7.7' ---
ArithmeticError: Bit shift by negative number
--- testing: '65' << 'abc' ---
TypeError: Unsupported operand types: string << string
--- testing: '65' << '123abc' ---
int(0)
--- testing: '65' << '123e5' ---
int(0)
--- testing: '65' << '123e5xyz' ---
int(0)
--- testing: '65' << ' 123abc' ---
int(0)
--- testing: '65' << '123 abc' ---
int(0)
--- testing: '65' << '123abc ' ---
int(0)
--- testing: '65' << '3.4a' ---
int(520)
--- testing: '65' << 'a5.9' ---
TypeError: Unsupported operand types: string << string
--- testing: '-44' << '0' ---
int(-44)
--- testing: '-44' << '65' ---
int(0)
--- testing: '-44' << '-44' ---
ArithmeticError: Bit shift by negative number
--- testing: '-44' << '1.2' ---
int(-88)
--- testing: '-44' << '-7.7' ---
ArithmeticError: Bit shift by negative number
--- testing: '-44' << 'abc' ---
TypeError: Unsupported operand types: string << string
--- testing: '-44' << '123abc' ---
int(0)
--- testing: '-44' << '123e5' ---
int(0)
--- testing: '-44' << '123e5xyz' ---
int(0)
--- testing: '-44' << ' 123abc' ---
int(0)
--- testing: '-44' << '123 abc' ---
int(0)
--- testing: '-44' << '123abc ' ---
int(0)
--- testing: '-44' << '3.4a' ---
int(-352)
--- testing: '-44' << 'a5.9' ---
TypeError: Unsupported operand types: string << string
--- testing: '1.2' << '0' ---
int(1)
--- testing: '1.2' << '65' ---
int(0)
--- testing: '1.2' << '-44' ---
ArithmeticError: Bit shift by negative number
--- testing: '1.2' << '1.2' ---
int(2)
--- testing: '1.2' << '-7.7' ---
ArithmeticError: Bit shift by negative number
--- testing: '1.2' << 'abc' ---
TypeError: Unsupported operand types: string << string
--- testing: '1.2' << '123abc' ---
int(0)
--- testing: '1.2' << '123e5' ---
int(0)
--- testing: '1.2' << '123e5xyz' ---
int(0)
--- testing: '1.2' << ' 123abc' ---
int(0)
--- testing: '1.2' << '123 abc' ---
int(0)
--- testing: '1.2' << '123abc ' ---
int(0)
--- testing: '1.2' << '3.4a' ---
int(8)
--- testing: '1.2' << 'a5.9' ---
TypeError: Unsupported operand types: string << string
--- testing: '-7.7' << '0' ---
int(-7)
--- testing: '-7.7' << '65' ---
int(0)
--- testing: '-7.7' << '-44' ---
ArithmeticError: Bit shift by negative number
--- testing: '-7.7' << '1.2' ---
int(-14)
--- testing: '-7.7' << '-7.7' ---
ArithmeticError: Bit shift by negative number
--- testing: '-7.7' << 'abc' ---
TypeError: Unsupported operand types: string << string
--- testing: '-7.7' << '123abc' ---
int(0)
--- testing: '-7.7' << '123e5' ---
int(0)
--- testing: '-7.7' << '123e5xyz' ---
int(0)
--- testing: '-7.7' << ' 123abc' ---
int(0)
--- testing: '-7.7' << '123 abc' ---
int(0)
--- testing: '-7.7' << '123abc ' ---
int(0)
--- testing: '-7.7' << '3.4a' ---
int(-56)
--- testing: '-7.7' << 'a5.9' ---
TypeError: Unsupported operand types: string << string
--- testing: 'abc' << '0' ---
TypeError: Unsupported operand types: string << string
--- testing: 'abc' << '65' ---
TypeError: Unsupported operand types: string << string
--- testing: 'abc' << '-44' ---
TypeError: Unsupported operand types: string << string
--- testing: 'abc' << '1.2' ---
TypeError: Unsupported operand types: string << string
--- testing: 'abc' << '-7.7' ---
TypeError: Unsupported operand types: string << string
--- testing: 'abc' << 'abc' ---
TypeError: Unsupported operand types: string << string
--- testing: 'abc' << '123abc' ---
TypeError: Unsupported operand types: string << string
--- testing: 'abc' << '123e5' ---
TypeError: Unsupported operand types: string << string
--- testing: 'abc' << '123e5xyz' ---
TypeError: Unsupported operand types: string << string
--- testing: 'abc' << ' 123abc' ---
TypeError: Unsupported operand types: string << string
--- testing: 'abc' << '123 abc' ---
TypeError: Unsupported operand types: string << string
--- testing: 'abc' << '123abc ' ---
TypeError: Unsupported operand types: string << string
--- testing: 'abc' << '3.4a' ---
TypeError: Unsupported operand types: string << string
--- testing: 'abc' << 'a5.9' ---
TypeError: Unsupported operand types: string << string
--- testing: '123abc' << '0' ---
int(123)
--- testing: '123abc' << '65' ---
int(0)
--- testing: '123abc' << '-44' ---
ArithmeticError: Bit shift by negative number
--- testing: '123abc' << '1.2' ---
int(246)
--- testing: '123abc' << '-7.7' ---
ArithmeticError: Bit shift by negative number
--- testing: '123abc' << 'abc' ---
TypeError: Unsupported operand types: string << string
--- testing: '123abc' << '123abc' ---
int(0)
--- testing: '123abc' << '123e5' ---
int(0)
--- testing: '123abc' << '123e5xyz' ---
int(0)
--- testing: '123abc' << ' 123abc' ---
int(0)
--- testing: '123abc' << '123 abc' ---
int(0)
--- testing: '123abc' << '123abc ' ---
int(0)
--- testing: '123abc' << '3.4a' ---
int(984)
--- testing: '123abc' << 'a5.9' ---
TypeError: Unsupported operand types: string << string
--- testing: '123e5' << '0' ---
int(12300000)
--- testing: '123e5' << '65' ---
int(0)
--- testing: '123e5' << '-44' ---
ArithmeticError: Bit shift by negative number
--- testing: '123e5' << '1.2' ---
int(24600000)
--- testing: '123e5' << '-7.7' ---
ArithmeticError: Bit shift by negative number
--- testing: '123e5' << 'abc' ---
TypeError: Unsupported operand types: string << string
--- testing: '123e5' << '123abc' ---
int(0)
--- testing: '123e5' << '123e5' ---
int(0)
--- testing: '123e5' << '123e5xyz' ---
int(0)
--- testing: '123e5' << ' 123abc' ---
int(0)
--- testing: '123e5' << '123 abc' ---
int(0)
--- testing: '123e5' << '123abc ' ---
int(0)
--- testing: '123e5' << '3.4a' ---
int(98400000)
--- testing: '123e5' << 'a5.9' ---
TypeError: Unsupported operand types: string << string
--- testing: '123e5xyz' << '0' ---
int(12300000)
--- testing: '123e5xyz' << '65' ---
int(0)
--- testing: '123e5xyz' << '-44' ---
ArithmeticError: Bit shift by negative number
--- testing: '123e5xyz' << '1.2' ---
int(24600000)
--- testing: '123e5xyz' << '-7.7' ---
ArithmeticError: Bit shift by negative number
--- testing: '123e5xyz' << 'abc' ---
TypeError: Unsupported operand types: string << string
--- testing: '123e5xyz' << '123abc' ---
int(0)
--- testing: '123e5xyz' << '123e5' ---
int(0)
--- testing: '123e5xyz' << '123e5xyz' ---
int(0)
--- testing: '123e5xyz' << ' 123abc' ---
int(0)
--- testing: '123e5xyz' << '123 abc' ---
int(0)
--- testing: '123e5xyz' << '123abc ' ---
int(0)
--- testing: '123e5xyz' << '3.4a' ---
int(98400000)
--- testing: '123e5xyz' << 'a5.9' ---
TypeError: Unsupported operand types: string << string
--- testing: ' 123abc' << '0' ---
int(123)
--- testing: ' 123abc' << '65' ---
int(0)
--- testing: ' 123abc' << '-44' ---
ArithmeticError: Bit shift by negative number
--- testing: ' 123abc' << '1.2' ---
int(246)
--- testing: ' 123abc' << '-7.7' ---
ArithmeticError: Bit shift by negative number
--- testing: ' 123abc' << 'abc' ---
TypeError: Unsupported operand types: string << string
--- testing: ' 123abc' << '123abc' ---
int(0)
--- testing: ' 123abc' << '123e5' ---
int(0)
--- testing: ' 123abc' << '123e5xyz' ---
int(0)
--- testing: ' 123abc' << ' 123abc' ---
int(0)
--- testing: ' 123abc' << '123 abc' ---
int(0)
--- testing: ' 123abc' << '123abc ' ---
int(0)
--- testing: ' 123abc' << '3.4a' ---
int(984)
--- testing: ' 123abc' << 'a5.9' ---
TypeError: Unsupported operand types: string << string
--- testing: '123 abc' << '0' ---
int(123)
--- testing: '123 abc' << '65' ---
int(0)
--- testing: '123 abc' << '-44' ---
ArithmeticError: Bit shift by negative number
--- testing: '123 abc' << '1.2' ---
int(246)
--- testing: '123 abc' << '-7.7' ---
ArithmeticError: Bit shift by negative number
--- testing: '123 abc' << 'abc' ---
TypeError: Unsupported operand types: string << string
--- testing: '123 abc' << '123abc' ---
int(0)
--- testing: '123 abc' << '123e5' ---
int(0)
--- testing: '123 abc' << '123e5xyz' ---
int(0)
--- testing: '123 abc' << ' 123abc' ---
int(0)
--- testing: '123 abc' << '123 abc' ---
int(0)
--- testing: '123 abc' << '123abc ' ---
int(0)
--- testing: '123 abc' << '3.4a' ---
int(984)
--- testing: '123 abc' << 'a5.9' ---
TypeError: Unsupported operand types: string << string
--- testing: '123abc ' << '0' ---
int(123)
--- testing: '123abc ' << '65' ---
int(0)
--- testing: '123abc ' << '-44' ---
ArithmeticError: Bit shift by negative number
--- testing: '123abc ' << '1.2' ---
int(246)
--- testing: '123abc ' << '-7.7' ---
ArithmeticError: Bit shift by negative number
--- testing: '123abc ' << 'abc' ---
TypeError: Unsupported operand types: string << string
--- testing: '123abc ' << '123abc' ---
int(0)
--- testing: '123abc ' << '123e5' ---
int(0)
--- testing: '123abc ' << '123e5xyz' ---
int(0)
--- testing: '123abc ' << ' 123abc' ---
int(0)
--- testing: '123abc ' << '123 abc' ---
int(0)
--- testing: '123abc ' << '123abc ' ---
int(0)
--- testing: '123abc ' << '3.4a' ---
int(984)
--- testing: '123abc ' << 'a5.9' ---
TypeError: Unsupported operand types: string << string
--- testing: '3.4a' << '0' ---
int(3)
--- testing: '3.4a' << '65' ---
int(0)
--- testing: '3.4a' << '-44' ---
ArithmeticError: Bit shift by negative number
--- testing: '3.4a' << '1.2' ---
int(6)
--- testing: '3.4a' << '-7.7' ---
ArithmeticError: Bit shift by negative number
--- testing: '3.4a' << 'abc' ---
TypeError: Unsupported operand types: string << string
--- testing: '3.4a' << '123abc' ---
int(0)
--- testing: '3.4a' << '123e5' ---
int(0)
--- testing: '3.4a' << '123e5xyz' ---
int(0)
--- testing: '3.4a' << ' 123abc' ---
int(0)
--- testing: '3.4a' << '123 abc' ---
int(0)
--- testing: '3.4a' << '123abc ' ---
int(0)
--- testing: '3.4a' << '3.4a' ---
int(24)
--- testing: '3.4a' << 'a5.9' ---
TypeError: Unsupported operand types: string << string
--- testing: 'a5.9' << '0' ---
TypeError: Unsupported operand types: string << string
--- testing: 'a5.9' << '65' ---
TypeError: Unsupported operand types: string << string
--- testing: 'a5.9' << '-44' ---
TypeError: Unsupported operand types: string << string
--- testing: 'a5.9' << '1.2' ---
TypeError: Unsupported operand types: string << string
--- testing: 'a5.9' << '-7.7' ---
TypeError: Unsupported operand types: string << string
--- testing: 'a5.9' << 'abc' ---
TypeError: Unsupported operand types: string << string
--- testing: 'a5.9' << '123abc' ---
TypeError: Unsupported operand types: string << string
--- testing: 'a5.9' << '123e5' ---
TypeError: Unsupported operand types: string << string
--- testing: 'a5.9' << '123e5xyz' ---
TypeError: Unsupported operand types: string << string
--- testing: 'a5.9' << ' 123abc' ---
TypeError: Unsupported operand types: string << string
--- testing: 'a5.9' << '123 abc' ---
TypeError: Unsupported operand types: string << string
--- testing: 'a5.9' << '123abc ' ---
TypeError: Unsupported operand types: string << string
--- testing: 'a5.9' << '3.4a' ---
TypeError: Unsupported operand types: string << string
--- testing: 'a5.9' << 'a5.9' ---
TypeError: Unsupported operand types: string << string
