--TEST--
Test / operator : various numbers as strings
--FILE--
<?php

$strVals = array(
   "0","65","-44", "1.2", "-7.7", "abc", "123abc", "123e5", "123e5xyz", " 123abc", "123 abc", "123abc ", "3.4a",
   "a5.9"
);

error_reporting(E_ERROR);

foreach ($strVals as $strVal) {
    foreach($strVals as $otherVal) {
        echo "--- testing: '$strVal'/'$otherVal' ---\n";
        try {
            var_dump($strVal/$otherVal);
        } catch (\TypeError $e) {
            echo $e->getMessage() . \PHP_EOL;
        } catch (\DivisionByZeroError $e) {
            echo $e->getMessage() . \PHP_EOL;
        }
    }
}


?>
--EXPECT--
--- testing: '0'/'0' ---
Division by zero
--- testing: '0'/'65' ---
int(0)
--- testing: '0'/'-44' ---
int(0)
--- testing: '0'/'1.2' ---
float(0)
--- testing: '0'/'-7.7' ---
float(-0)
--- testing: '0'/'abc' ---
Unsupported operand types: string / string
--- testing: '0'/'123abc' ---
int(0)
--- testing: '0'/'123e5' ---
float(0)
--- testing: '0'/'123e5xyz' ---
float(0)
--- testing: '0'/' 123abc' ---
int(0)
--- testing: '0'/'123 abc' ---
int(0)
--- testing: '0'/'123abc ' ---
int(0)
--- testing: '0'/'3.4a' ---
float(0)
--- testing: '0'/'a5.9' ---
Unsupported operand types: string / string
--- testing: '65'/'0' ---
Division by zero
--- testing: '65'/'65' ---
int(1)
--- testing: '65'/'-44' ---
float(-1.4772727272727273)
--- testing: '65'/'1.2' ---
float(54.16666666666667)
--- testing: '65'/'-7.7' ---
float(-8.441558441558442)
--- testing: '65'/'abc' ---
Unsupported operand types: string / string
--- testing: '65'/'123abc' ---
float(0.5284552845528455)
--- testing: '65'/'123e5' ---
float(5.2845528455284555E-6)
--- testing: '65'/'123e5xyz' ---
float(5.2845528455284555E-6)
--- testing: '65'/' 123abc' ---
float(0.5284552845528455)
--- testing: '65'/'123 abc' ---
float(0.5284552845528455)
--- testing: '65'/'123abc ' ---
float(0.5284552845528455)
--- testing: '65'/'3.4a' ---
float(19.11764705882353)
--- testing: '65'/'a5.9' ---
Unsupported operand types: string / string
--- testing: '-44'/'0' ---
Division by zero
--- testing: '-44'/'65' ---
float(-0.676923076923077)
--- testing: '-44'/'-44' ---
int(1)
--- testing: '-44'/'1.2' ---
float(-36.66666666666667)
--- testing: '-44'/'-7.7' ---
float(5.714285714285714)
--- testing: '-44'/'abc' ---
Unsupported operand types: string / string
--- testing: '-44'/'123abc' ---
float(-0.35772357723577236)
--- testing: '-44'/'123e5' ---
float(-3.5772357723577236E-6)
--- testing: '-44'/'123e5xyz' ---
float(-3.5772357723577236E-6)
--- testing: '-44'/' 123abc' ---
float(-0.35772357723577236)
--- testing: '-44'/'123 abc' ---
float(-0.35772357723577236)
--- testing: '-44'/'123abc ' ---
float(-0.35772357723577236)
--- testing: '-44'/'3.4a' ---
float(-12.941176470588236)
--- testing: '-44'/'a5.9' ---
Unsupported operand types: string / string
--- testing: '1.2'/'0' ---
Division by zero
--- testing: '1.2'/'65' ---
float(0.01846153846153846)
--- testing: '1.2'/'-44' ---
float(-0.02727272727272727)
--- testing: '1.2'/'1.2' ---
float(1)
--- testing: '1.2'/'-7.7' ---
float(-0.15584415584415584)
--- testing: '1.2'/'abc' ---
Unsupported operand types: string / string
--- testing: '1.2'/'123abc' ---
float(0.00975609756097561)
--- testing: '1.2'/'123e5' ---
float(9.75609756097561E-8)
--- testing: '1.2'/'123e5xyz' ---
float(9.75609756097561E-8)
--- testing: '1.2'/' 123abc' ---
float(0.00975609756097561)
--- testing: '1.2'/'123 abc' ---
float(0.00975609756097561)
--- testing: '1.2'/'123abc ' ---
float(0.00975609756097561)
--- testing: '1.2'/'3.4a' ---
float(0.35294117647058826)
--- testing: '1.2'/'a5.9' ---
Unsupported operand types: string / string
--- testing: '-7.7'/'0' ---
Division by zero
--- testing: '-7.7'/'65' ---
float(-0.11846153846153847)
--- testing: '-7.7'/'-44' ---
float(0.17500000000000002)
--- testing: '-7.7'/'1.2' ---
float(-6.416666666666667)
--- testing: '-7.7'/'-7.7' ---
float(1)
--- testing: '-7.7'/'abc' ---
Unsupported operand types: string / string
--- testing: '-7.7'/'123abc' ---
float(-0.06260162601626017)
--- testing: '-7.7'/'123e5' ---
float(-6.260162601626017E-7)
--- testing: '-7.7'/'123e5xyz' ---
float(-6.260162601626017E-7)
--- testing: '-7.7'/' 123abc' ---
float(-0.06260162601626017)
--- testing: '-7.7'/'123 abc' ---
float(-0.06260162601626017)
--- testing: '-7.7'/'123abc ' ---
float(-0.06260162601626017)
--- testing: '-7.7'/'3.4a' ---
float(-2.264705882352941)
--- testing: '-7.7'/'a5.9' ---
Unsupported operand types: string / string
--- testing: 'abc'/'0' ---
Unsupported operand types: string / string
--- testing: 'abc'/'65' ---
Unsupported operand types: string / string
--- testing: 'abc'/'-44' ---
Unsupported operand types: string / string
--- testing: 'abc'/'1.2' ---
Unsupported operand types: string / string
--- testing: 'abc'/'-7.7' ---
Unsupported operand types: string / string
--- testing: 'abc'/'abc' ---
Unsupported operand types: string / string
--- testing: 'abc'/'123abc' ---
Unsupported operand types: string / string
--- testing: 'abc'/'123e5' ---
Unsupported operand types: string / string
--- testing: 'abc'/'123e5xyz' ---
Unsupported operand types: string / string
--- testing: 'abc'/' 123abc' ---
Unsupported operand types: string / string
--- testing: 'abc'/'123 abc' ---
Unsupported operand types: string / string
--- testing: 'abc'/'123abc ' ---
Unsupported operand types: string / string
--- testing: 'abc'/'3.4a' ---
Unsupported operand types: string / string
--- testing: 'abc'/'a5.9' ---
Unsupported operand types: string / string
--- testing: '123abc'/'0' ---
Division by zero
--- testing: '123abc'/'65' ---
float(1.8923076923076922)
--- testing: '123abc'/'-44' ---
float(-2.7954545454545454)
--- testing: '123abc'/'1.2' ---
float(102.5)
--- testing: '123abc'/'-7.7' ---
float(-15.974025974025974)
--- testing: '123abc'/'abc' ---
Unsupported operand types: string / string
--- testing: '123abc'/'123abc' ---
int(1)
--- testing: '123abc'/'123e5' ---
float(1.0E-5)
--- testing: '123abc'/'123e5xyz' ---
float(1.0E-5)
--- testing: '123abc'/' 123abc' ---
int(1)
--- testing: '123abc'/'123 abc' ---
int(1)
--- testing: '123abc'/'123abc ' ---
int(1)
--- testing: '123abc'/'3.4a' ---
float(36.1764705882353)
--- testing: '123abc'/'a5.9' ---
Unsupported operand types: string / string
--- testing: '123e5'/'0' ---
Division by zero
--- testing: '123e5'/'65' ---
float(189230.76923076922)
--- testing: '123e5'/'-44' ---
float(-279545.45454545453)
--- testing: '123e5'/'1.2' ---
float(10250000)
--- testing: '123e5'/'-7.7' ---
float(-1597402.5974025973)
--- testing: '123e5'/'abc' ---
Unsupported operand types: string / string
--- testing: '123e5'/'123abc' ---
float(100000)
--- testing: '123e5'/'123e5' ---
float(1)
--- testing: '123e5'/'123e5xyz' ---
float(1)
--- testing: '123e5'/' 123abc' ---
float(100000)
--- testing: '123e5'/'123 abc' ---
float(100000)
--- testing: '123e5'/'123abc ' ---
float(100000)
--- testing: '123e5'/'3.4a' ---
float(3617647.0588235296)
--- testing: '123e5'/'a5.9' ---
Unsupported operand types: string / string
--- testing: '123e5xyz'/'0' ---
Division by zero
--- testing: '123e5xyz'/'65' ---
float(189230.76923076922)
--- testing: '123e5xyz'/'-44' ---
float(-279545.45454545453)
--- testing: '123e5xyz'/'1.2' ---
float(10250000)
--- testing: '123e5xyz'/'-7.7' ---
float(-1597402.5974025973)
--- testing: '123e5xyz'/'abc' ---
Unsupported operand types: string / string
--- testing: '123e5xyz'/'123abc' ---
float(100000)
--- testing: '123e5xyz'/'123e5' ---
float(1)
--- testing: '123e5xyz'/'123e5xyz' ---
float(1)
--- testing: '123e5xyz'/' 123abc' ---
float(100000)
--- testing: '123e5xyz'/'123 abc' ---
float(100000)
--- testing: '123e5xyz'/'123abc ' ---
float(100000)
--- testing: '123e5xyz'/'3.4a' ---
float(3617647.0588235296)
--- testing: '123e5xyz'/'a5.9' ---
Unsupported operand types: string / string
--- testing: ' 123abc'/'0' ---
Division by zero
--- testing: ' 123abc'/'65' ---
float(1.8923076923076922)
--- testing: ' 123abc'/'-44' ---
float(-2.7954545454545454)
--- testing: ' 123abc'/'1.2' ---
float(102.5)
--- testing: ' 123abc'/'-7.7' ---
float(-15.974025974025974)
--- testing: ' 123abc'/'abc' ---
Unsupported operand types: string / string
--- testing: ' 123abc'/'123abc' ---
int(1)
--- testing: ' 123abc'/'123e5' ---
float(1.0E-5)
--- testing: ' 123abc'/'123e5xyz' ---
float(1.0E-5)
--- testing: ' 123abc'/' 123abc' ---
int(1)
--- testing: ' 123abc'/'123 abc' ---
int(1)
--- testing: ' 123abc'/'123abc ' ---
int(1)
--- testing: ' 123abc'/'3.4a' ---
float(36.1764705882353)
--- testing: ' 123abc'/'a5.9' ---
Unsupported operand types: string / string
--- testing: '123 abc'/'0' ---
Division by zero
--- testing: '123 abc'/'65' ---
float(1.8923076923076922)
--- testing: '123 abc'/'-44' ---
float(-2.7954545454545454)
--- testing: '123 abc'/'1.2' ---
float(102.5)
--- testing: '123 abc'/'-7.7' ---
float(-15.974025974025974)
--- testing: '123 abc'/'abc' ---
Unsupported operand types: string / string
--- testing: '123 abc'/'123abc' ---
int(1)
--- testing: '123 abc'/'123e5' ---
float(1.0E-5)
--- testing: '123 abc'/'123e5xyz' ---
float(1.0E-5)
--- testing: '123 abc'/' 123abc' ---
int(1)
--- testing: '123 abc'/'123 abc' ---
int(1)
--- testing: '123 abc'/'123abc ' ---
int(1)
--- testing: '123 abc'/'3.4a' ---
float(36.1764705882353)
--- testing: '123 abc'/'a5.9' ---
Unsupported operand types: string / string
--- testing: '123abc '/'0' ---
Division by zero
--- testing: '123abc '/'65' ---
float(1.8923076923076922)
--- testing: '123abc '/'-44' ---
float(-2.7954545454545454)
--- testing: '123abc '/'1.2' ---
float(102.5)
--- testing: '123abc '/'-7.7' ---
float(-15.974025974025974)
--- testing: '123abc '/'abc' ---
Unsupported operand types: string / string
--- testing: '123abc '/'123abc' ---
int(1)
--- testing: '123abc '/'123e5' ---
float(1.0E-5)
--- testing: '123abc '/'123e5xyz' ---
float(1.0E-5)
--- testing: '123abc '/' 123abc' ---
int(1)
--- testing: '123abc '/'123 abc' ---
int(1)
--- testing: '123abc '/'123abc ' ---
int(1)
--- testing: '123abc '/'3.4a' ---
float(36.1764705882353)
--- testing: '123abc '/'a5.9' ---
Unsupported operand types: string / string
--- testing: '3.4a'/'0' ---
Division by zero
--- testing: '3.4a'/'65' ---
float(0.052307692307692305)
--- testing: '3.4a'/'-44' ---
float(-0.07727272727272727)
--- testing: '3.4a'/'1.2' ---
float(2.8333333333333335)
--- testing: '3.4a'/'-7.7' ---
float(-0.44155844155844154)
--- testing: '3.4a'/'abc' ---
Unsupported operand types: string / string
--- testing: '3.4a'/'123abc' ---
float(0.027642276422764227)
--- testing: '3.4a'/'123e5' ---
float(2.764227642276423E-7)
--- testing: '3.4a'/'123e5xyz' ---
float(2.764227642276423E-7)
--- testing: '3.4a'/' 123abc' ---
float(0.027642276422764227)
--- testing: '3.4a'/'123 abc' ---
float(0.027642276422764227)
--- testing: '3.4a'/'123abc ' ---
float(0.027642276422764227)
--- testing: '3.4a'/'3.4a' ---
float(1)
--- testing: '3.4a'/'a5.9' ---
Unsupported operand types: string / string
--- testing: 'a5.9'/'0' ---
Unsupported operand types: string / string
--- testing: 'a5.9'/'65' ---
Unsupported operand types: string / string
--- testing: 'a5.9'/'-44' ---
Unsupported operand types: string / string
--- testing: 'a5.9'/'1.2' ---
Unsupported operand types: string / string
--- testing: 'a5.9'/'-7.7' ---
Unsupported operand types: string / string
--- testing: 'a5.9'/'abc' ---
Unsupported operand types: string / string
--- testing: 'a5.9'/'123abc' ---
Unsupported operand types: string / string
--- testing: 'a5.9'/'123e5' ---
Unsupported operand types: string / string
--- testing: 'a5.9'/'123e5xyz' ---
Unsupported operand types: string / string
--- testing: 'a5.9'/' 123abc' ---
Unsupported operand types: string / string
--- testing: 'a5.9'/'123 abc' ---
Unsupported operand types: string / string
--- testing: 'a5.9'/'123abc ' ---
Unsupported operand types: string / string
--- testing: 'a5.9'/'3.4a' ---
Unsupported operand types: string / string
--- testing: 'a5.9'/'a5.9' ---
Unsupported operand types: string / string
