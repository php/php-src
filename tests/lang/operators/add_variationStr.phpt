--TEST--
Test + operator : various numbers as strings
--FILE--
<?php

$strVals = array(
   "0","65","-44", "1.2", "-7.7", "abc", "123abc", "123e5", "123e5xyz", " 123abc", "123 abc", "123abc ", "3.4a",
   "a5.9"
);

error_reporting(E_ERROR);

foreach ($strVals as $strVal) {
    foreach($strVals as $otherVal) {
        echo "--- testing: '$strVal' + '$otherVal' ---\n";
        try {
            var_dump($strVal+$otherVal);
        } catch (\TypeError $e) {
            echo $e->getMessage() . \PHP_EOL;
        }
    }
}

?>
--EXPECT--
--- testing: '0' + '0' ---
int(0)
--- testing: '0' + '65' ---
int(65)
--- testing: '0' + '-44' ---
int(-44)
--- testing: '0' + '1.2' ---
float(1.2)
--- testing: '0' + '-7.7' ---
float(-7.7)
--- testing: '0' + 'abc' ---
Unsupported operand types: string + string
--- testing: '0' + '123abc' ---
int(123)
--- testing: '0' + '123e5' ---
float(12300000)
--- testing: '0' + '123e5xyz' ---
float(12300000)
--- testing: '0' + ' 123abc' ---
int(123)
--- testing: '0' + '123 abc' ---
int(123)
--- testing: '0' + '123abc ' ---
int(123)
--- testing: '0' + '3.4a' ---
float(3.4)
--- testing: '0' + 'a5.9' ---
Unsupported operand types: string + string
--- testing: '65' + '0' ---
int(65)
--- testing: '65' + '65' ---
int(130)
--- testing: '65' + '-44' ---
int(21)
--- testing: '65' + '1.2' ---
float(66.2)
--- testing: '65' + '-7.7' ---
float(57.3)
--- testing: '65' + 'abc' ---
Unsupported operand types: string + string
--- testing: '65' + '123abc' ---
int(188)
--- testing: '65' + '123e5' ---
float(12300065)
--- testing: '65' + '123e5xyz' ---
float(12300065)
--- testing: '65' + ' 123abc' ---
int(188)
--- testing: '65' + '123 abc' ---
int(188)
--- testing: '65' + '123abc ' ---
int(188)
--- testing: '65' + '3.4a' ---
float(68.4)
--- testing: '65' + 'a5.9' ---
Unsupported operand types: string + string
--- testing: '-44' + '0' ---
int(-44)
--- testing: '-44' + '65' ---
int(21)
--- testing: '-44' + '-44' ---
int(-88)
--- testing: '-44' + '1.2' ---
float(-42.8)
--- testing: '-44' + '-7.7' ---
float(-51.7)
--- testing: '-44' + 'abc' ---
Unsupported operand types: string + string
--- testing: '-44' + '123abc' ---
int(79)
--- testing: '-44' + '123e5' ---
float(12299956)
--- testing: '-44' + '123e5xyz' ---
float(12299956)
--- testing: '-44' + ' 123abc' ---
int(79)
--- testing: '-44' + '123 abc' ---
int(79)
--- testing: '-44' + '123abc ' ---
int(79)
--- testing: '-44' + '3.4a' ---
float(-40.6)
--- testing: '-44' + 'a5.9' ---
Unsupported operand types: string + string
--- testing: '1.2' + '0' ---
float(1.2)
--- testing: '1.2' + '65' ---
float(66.2)
--- testing: '1.2' + '-44' ---
float(-42.8)
--- testing: '1.2' + '1.2' ---
float(2.4)
--- testing: '1.2' + '-7.7' ---
float(-6.5)
--- testing: '1.2' + 'abc' ---
Unsupported operand types: string + string
--- testing: '1.2' + '123abc' ---
float(124.2)
--- testing: '1.2' + '123e5' ---
float(12300001.2)
--- testing: '1.2' + '123e5xyz' ---
float(12300001.2)
--- testing: '1.2' + ' 123abc' ---
float(124.2)
--- testing: '1.2' + '123 abc' ---
float(124.2)
--- testing: '1.2' + '123abc ' ---
float(124.2)
--- testing: '1.2' + '3.4a' ---
float(4.6)
--- testing: '1.2' + 'a5.9' ---
Unsupported operand types: string + string
--- testing: '-7.7' + '0' ---
float(-7.7)
--- testing: '-7.7' + '65' ---
float(57.3)
--- testing: '-7.7' + '-44' ---
float(-51.7)
--- testing: '-7.7' + '1.2' ---
float(-6.5)
--- testing: '-7.7' + '-7.7' ---
float(-15.4)
--- testing: '-7.7' + 'abc' ---
Unsupported operand types: string + string
--- testing: '-7.7' + '123abc' ---
float(115.3)
--- testing: '-7.7' + '123e5' ---
float(12299992.3)
--- testing: '-7.7' + '123e5xyz' ---
float(12299992.3)
--- testing: '-7.7' + ' 123abc' ---
float(115.3)
--- testing: '-7.7' + '123 abc' ---
float(115.3)
--- testing: '-7.7' + '123abc ' ---
float(115.3)
--- testing: '-7.7' + '3.4a' ---
float(-4.300000000000001)
--- testing: '-7.7' + 'a5.9' ---
Unsupported operand types: string + string
--- testing: 'abc' + '0' ---
Unsupported operand types: string + string
--- testing: 'abc' + '65' ---
Unsupported operand types: string + string
--- testing: 'abc' + '-44' ---
Unsupported operand types: string + string
--- testing: 'abc' + '1.2' ---
Unsupported operand types: string + string
--- testing: 'abc' + '-7.7' ---
Unsupported operand types: string + string
--- testing: 'abc' + 'abc' ---
Unsupported operand types: string + string
--- testing: 'abc' + '123abc' ---
Unsupported operand types: string + string
--- testing: 'abc' + '123e5' ---
Unsupported operand types: string + string
--- testing: 'abc' + '123e5xyz' ---
Unsupported operand types: string + string
--- testing: 'abc' + ' 123abc' ---
Unsupported operand types: string + string
--- testing: 'abc' + '123 abc' ---
Unsupported operand types: string + string
--- testing: 'abc' + '123abc ' ---
Unsupported operand types: string + string
--- testing: 'abc' + '3.4a' ---
Unsupported operand types: string + string
--- testing: 'abc' + 'a5.9' ---
Unsupported operand types: string + string
--- testing: '123abc' + '0' ---
int(123)
--- testing: '123abc' + '65' ---
int(188)
--- testing: '123abc' + '-44' ---
int(79)
--- testing: '123abc' + '1.2' ---
float(124.2)
--- testing: '123abc' + '-7.7' ---
float(115.3)
--- testing: '123abc' + 'abc' ---
Unsupported operand types: string + string
--- testing: '123abc' + '123abc' ---
int(246)
--- testing: '123abc' + '123e5' ---
float(12300123)
--- testing: '123abc' + '123e5xyz' ---
float(12300123)
--- testing: '123abc' + ' 123abc' ---
int(246)
--- testing: '123abc' + '123 abc' ---
int(246)
--- testing: '123abc' + '123abc ' ---
int(246)
--- testing: '123abc' + '3.4a' ---
float(126.4)
--- testing: '123abc' + 'a5.9' ---
Unsupported operand types: string + string
--- testing: '123e5' + '0' ---
float(12300000)
--- testing: '123e5' + '65' ---
float(12300065)
--- testing: '123e5' + '-44' ---
float(12299956)
--- testing: '123e5' + '1.2' ---
float(12300001.2)
--- testing: '123e5' + '-7.7' ---
float(12299992.3)
--- testing: '123e5' + 'abc' ---
Unsupported operand types: string + string
--- testing: '123e5' + '123abc' ---
float(12300123)
--- testing: '123e5' + '123e5' ---
float(24600000)
--- testing: '123e5' + '123e5xyz' ---
float(24600000)
--- testing: '123e5' + ' 123abc' ---
float(12300123)
--- testing: '123e5' + '123 abc' ---
float(12300123)
--- testing: '123e5' + '123abc ' ---
float(12300123)
--- testing: '123e5' + '3.4a' ---
float(12300003.4)
--- testing: '123e5' + 'a5.9' ---
Unsupported operand types: string + string
--- testing: '123e5xyz' + '0' ---
float(12300000)
--- testing: '123e5xyz' + '65' ---
float(12300065)
--- testing: '123e5xyz' + '-44' ---
float(12299956)
--- testing: '123e5xyz' + '1.2' ---
float(12300001.2)
--- testing: '123e5xyz' + '-7.7' ---
float(12299992.3)
--- testing: '123e5xyz' + 'abc' ---
Unsupported operand types: string + string
--- testing: '123e5xyz' + '123abc' ---
float(12300123)
--- testing: '123e5xyz' + '123e5' ---
float(24600000)
--- testing: '123e5xyz' + '123e5xyz' ---
float(24600000)
--- testing: '123e5xyz' + ' 123abc' ---
float(12300123)
--- testing: '123e5xyz' + '123 abc' ---
float(12300123)
--- testing: '123e5xyz' + '123abc ' ---
float(12300123)
--- testing: '123e5xyz' + '3.4a' ---
float(12300003.4)
--- testing: '123e5xyz' + 'a5.9' ---
Unsupported operand types: string + string
--- testing: ' 123abc' + '0' ---
int(123)
--- testing: ' 123abc' + '65' ---
int(188)
--- testing: ' 123abc' + '-44' ---
int(79)
--- testing: ' 123abc' + '1.2' ---
float(124.2)
--- testing: ' 123abc' + '-7.7' ---
float(115.3)
--- testing: ' 123abc' + 'abc' ---
Unsupported operand types: string + string
--- testing: ' 123abc' + '123abc' ---
int(246)
--- testing: ' 123abc' + '123e5' ---
float(12300123)
--- testing: ' 123abc' + '123e5xyz' ---
float(12300123)
--- testing: ' 123abc' + ' 123abc' ---
int(246)
--- testing: ' 123abc' + '123 abc' ---
int(246)
--- testing: ' 123abc' + '123abc ' ---
int(246)
--- testing: ' 123abc' + '3.4a' ---
float(126.4)
--- testing: ' 123abc' + 'a5.9' ---
Unsupported operand types: string + string
--- testing: '123 abc' + '0' ---
int(123)
--- testing: '123 abc' + '65' ---
int(188)
--- testing: '123 abc' + '-44' ---
int(79)
--- testing: '123 abc' + '1.2' ---
float(124.2)
--- testing: '123 abc' + '-7.7' ---
float(115.3)
--- testing: '123 abc' + 'abc' ---
Unsupported operand types: string + string
--- testing: '123 abc' + '123abc' ---
int(246)
--- testing: '123 abc' + '123e5' ---
float(12300123)
--- testing: '123 abc' + '123e5xyz' ---
float(12300123)
--- testing: '123 abc' + ' 123abc' ---
int(246)
--- testing: '123 abc' + '123 abc' ---
int(246)
--- testing: '123 abc' + '123abc ' ---
int(246)
--- testing: '123 abc' + '3.4a' ---
float(126.4)
--- testing: '123 abc' + 'a5.9' ---
Unsupported operand types: string + string
--- testing: '123abc ' + '0' ---
int(123)
--- testing: '123abc ' + '65' ---
int(188)
--- testing: '123abc ' + '-44' ---
int(79)
--- testing: '123abc ' + '1.2' ---
float(124.2)
--- testing: '123abc ' + '-7.7' ---
float(115.3)
--- testing: '123abc ' + 'abc' ---
Unsupported operand types: string + string
--- testing: '123abc ' + '123abc' ---
int(246)
--- testing: '123abc ' + '123e5' ---
float(12300123)
--- testing: '123abc ' + '123e5xyz' ---
float(12300123)
--- testing: '123abc ' + ' 123abc' ---
int(246)
--- testing: '123abc ' + '123 abc' ---
int(246)
--- testing: '123abc ' + '123abc ' ---
int(246)
--- testing: '123abc ' + '3.4a' ---
float(126.4)
--- testing: '123abc ' + 'a5.9' ---
Unsupported operand types: string + string
--- testing: '3.4a' + '0' ---
float(3.4)
--- testing: '3.4a' + '65' ---
float(68.4)
--- testing: '3.4a' + '-44' ---
float(-40.6)
--- testing: '3.4a' + '1.2' ---
float(4.6)
--- testing: '3.4a' + '-7.7' ---
float(-4.300000000000001)
--- testing: '3.4a' + 'abc' ---
Unsupported operand types: string + string
--- testing: '3.4a' + '123abc' ---
float(126.4)
--- testing: '3.4a' + '123e5' ---
float(12300003.4)
--- testing: '3.4a' + '123e5xyz' ---
float(12300003.4)
--- testing: '3.4a' + ' 123abc' ---
float(126.4)
--- testing: '3.4a' + '123 abc' ---
float(126.4)
--- testing: '3.4a' + '123abc ' ---
float(126.4)
--- testing: '3.4a' + '3.4a' ---
float(6.8)
--- testing: '3.4a' + 'a5.9' ---
Unsupported operand types: string + string
--- testing: 'a5.9' + '0' ---
Unsupported operand types: string + string
--- testing: 'a5.9' + '65' ---
Unsupported operand types: string + string
--- testing: 'a5.9' + '-44' ---
Unsupported operand types: string + string
--- testing: 'a5.9' + '1.2' ---
Unsupported operand types: string + string
--- testing: 'a5.9' + '-7.7' ---
Unsupported operand types: string + string
--- testing: 'a5.9' + 'abc' ---
Unsupported operand types: string + string
--- testing: 'a5.9' + '123abc' ---
Unsupported operand types: string + string
--- testing: 'a5.9' + '123e5' ---
Unsupported operand types: string + string
--- testing: 'a5.9' + '123e5xyz' ---
Unsupported operand types: string + string
--- testing: 'a5.9' + ' 123abc' ---
Unsupported operand types: string + string
--- testing: 'a5.9' + '123 abc' ---
Unsupported operand types: string + string
--- testing: 'a5.9' + '123abc ' ---
Unsupported operand types: string + string
--- testing: 'a5.9' + '3.4a' ---
Unsupported operand types: string + string
--- testing: 'a5.9' + 'a5.9' ---
Unsupported operand types: string + string
