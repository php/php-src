--TEST--
Test round() function :  PHP_ROUND_UP/PHP_ROUND_DOWN modes
--FILE--
<?php
/* Prototype  : float round  ( float $val  [, int $precision  ] [, int mode  ] )
 * Description: Returns the rounded value of val to specified precision (number of digits
 * after the decimal point) according to the specified mode
 * Source code: ext/standard/math.c
 */
 
/*
 * Test behaviour of PHP_ROUND_UP/PHP_ROUND_DOWN modes
 */
 
echo "*** Testing round() : PHP_ROUND_UP/PHP_ROUND_DOWN modes ***", PHP_EOL, PHP_EOL;

$testData = [
    [3.45, 0, 3, 4],
    [3.50, 0, 3, 4],
    [3.55, 0, 3, 4],
    [-3.45, 0, -4, -3],
    [-3.50, 0, -4, -3],
    [-3.55, 0, -4, -3],
    [345, -1, 340, 350],
    [350, -1, 350, 350],
    [355, -1, 350, 360],
    [-345, -1, -350, -340],
    [-350, -1, -350, -350],
    [-355, -1, -360, -350],
    [345, -2, 300, 400],
    [350, -2, 300, 400],
    [355, -2, 300, 400],
    [-345, -2, -400, -300],
    [-350, -2, -400, -300],
    [-355, -2, -400, -300],
    [0.345, 1, 0.3, 0.4],
    [0.350, 1, 0.3, 0.4],
    [0.355, 1, 0.3, 0.4],
    [-0.345, 1, -0.4, -0.3],
    [-0.350, 1, -0.4, -0.3],
    [-0.355, 1, -0.4, -0.3],
    [0.345, 2, 0.34, 0.35],
    [0.350, 2, 0.35, 0.35],
    [0.355, 2, 0.35, 0.36],
    [-0.345, 2, -0.35, -0.34],
    [-0.350, 2, -0.35, -0.35],
    [-0.355, 2, -0.36, -0.35],
    [1.23456789e15, -12, 1.234e15, 1.235e15],
    [1.23456789e-15, 18, 1.234e-15, 1.235e-15],
    [-1.23456789e15, -12, -1.235e15, -1.234e15],
    [-1.23456789e-15, 18, -1.235e-15, -1.234e-15],
    [1.23456789e135, -132, 1.234e135, 1.235e135],
    [1.23456789e-135, 138, 1.234e-135, 1.235e-135],
    [-1.23456789e135, -132, -1.235e135, -1.234e135],
    [-1.23456789e-135, 138, -1.235e-135, -1.234e-135],
];

foreach($testData as $test) {
    list($value, $precision, $expected) = $test;
    $answer = round($value, $precision, PHP_ROUND_DOWN);
    echo $value, ' rounded down to ', $precision, ' precision is ', $answer, ' (', $expected, ') ', ($expected == $answer ? 'YES' : 'NO'), PHP_EOL;
}

echo PHP_EOL;

foreach($testData as $test) {
    list($value, $precision, , $expected) = $test;
    $answer = round($value, $precision, PHP_ROUND_UP);
    echo $value, ' rounded up to ', $precision, ' precision is ', $answer, ' (', $expected, ') ', ($expected == $answer ? 'YES' : 'NO'), PHP_EOL;
}

echo PHP_EOL;
?>
===Done===
--EXPECTF--
*** Testing round() : PHP_ROUND_UP/PHP_ROUND_DOWN modes ***

3.45 rounded down to 0 precision is 3 (3) YES
3.5 rounded down to 0 precision is 3 (3) YES
3.55 rounded down to 0 precision is 3 (3) YES
-3.45 rounded down to 0 precision is -4 (-4) YES
-3.5 rounded down to 0 precision is -4 (-4) YES
-3.55 rounded down to 0 precision is -4 (-4) YES
345 rounded down to -1 precision is 340 (340) YES
350 rounded down to -1 precision is 350 (350) YES
355 rounded down to -1 precision is 350 (350) YES
-345 rounded down to -1 precision is -350 (-350) YES
-350 rounded down to -1 precision is -350 (-350) YES
-355 rounded down to -1 precision is -360 (-360) YES
345 rounded down to -2 precision is 300 (300) YES
350 rounded down to -2 precision is 300 (300) YES
355 rounded down to -2 precision is 300 (300) YES
-345 rounded down to -2 precision is -400 (-400) YES
-350 rounded down to -2 precision is -400 (-400) YES
-355 rounded down to -2 precision is -400 (-400) YES
0.345 rounded down to 1 precision is 0.3 (0.3) YES
0.35 rounded down to 1 precision is 0.3 (0.3) YES
0.355 rounded down to 1 precision is 0.3 (0.3) YES
-0.345 rounded down to 1 precision is -0.4 (-0.4) YES
-0.35 rounded down to 1 precision is -0.4 (-0.4) YES
-0.355 rounded down to 1 precision is -0.4 (-0.4) YES
0.345 rounded down to 2 precision is 0.34 (0.34) YES
0.35 rounded down to 2 precision is 0.35 (0.35) YES
0.355 rounded down to 2 precision is 0.35 (0.35) YES
-0.345 rounded down to 2 precision is -0.35 (-0.35) YES
-0.35 rounded down to 2 precision is -0.35 (-0.35) YES
-0.355 rounded down to 2 precision is -0.36 (-0.36) YES
1.23456789E+15 rounded down to -12 precision is 1.234E+15 (1.234E+15) YES
1.23456789E-15 rounded down to 18 precision is 1.234E-15 (1.234E-15) YES
-1.23456789E+15 rounded down to -12 precision is -1.235E+15 (-1.235E+15) YES
-1.23456789E-15 rounded down to 18 precision is -1.235E-15 (-1.235E-15) YES
1.23456789E+135 rounded down to -132 precision is 1.234E+135 (1.234E+135) YES
1.23456789E-135 rounded down to 138 precision is 1.234E-135 (1.234E-135) YES
-1.23456789E+135 rounded down to -132 precision is -1.235E+135 (-1.235E+135) YES
-1.23456789E-135 rounded down to 138 precision is -1.235E-135 (-1.235E-135) YES

3.45 rounded up to 0 precision is 4 (4) YES
3.5 rounded up to 0 precision is 4 (4) YES
3.55 rounded up to 0 precision is 4 (4) YES
-3.45 rounded up to 0 precision is -3 (-3) YES
-3.5 rounded up to 0 precision is -3 (-3) YES
-3.55 rounded up to 0 precision is -3 (-3) YES
345 rounded up to -1 precision is 350 (350) YES
350 rounded up to -1 precision is 350 (350) YES
355 rounded up to -1 precision is 360 (360) YES
-345 rounded up to -1 precision is -340 (-340) YES
-350 rounded up to -1 precision is -350 (-350) YES
-355 rounded up to -1 precision is -350 (-350) YES
345 rounded up to -2 precision is 400 (400) YES
350 rounded up to -2 precision is 400 (400) YES
355 rounded up to -2 precision is 400 (400) YES
-345 rounded up to -2 precision is -300 (-300) YES
-350 rounded up to -2 precision is -300 (-300) YES
-355 rounded up to -2 precision is -300 (-300) YES
0.345 rounded up to 1 precision is 0.4 (0.4) YES
0.35 rounded up to 1 precision is 0.4 (0.4) YES
0.355 rounded up to 1 precision is 0.4 (0.4) YES
-0.345 rounded up to 1 precision is -0.3 (-0.3) YES
-0.35 rounded up to 1 precision is -0.3 (-0.3) YES
-0.355 rounded up to 1 precision is -0.3 (-0.3) YES
0.345 rounded up to 2 precision is 0.35 (0.35) YES
0.35 rounded up to 2 precision is 0.35 (0.35) YES
0.355 rounded up to 2 precision is 0.36 (0.36) YES
-0.345 rounded up to 2 precision is -0.34 (-0.34) YES
-0.35 rounded up to 2 precision is -0.35 (-0.35) YES
-0.355 rounded up to 2 precision is -0.35 (-0.35) YES
1.23456789E+15 rounded up to -12 precision is 1.235E+15 (1.235E+15) YES
1.23456789E-15 rounded up to 18 precision is 1.235E-15 (1.235E-15) YES
-1.23456789E+15 rounded up to -12 precision is -1.234E+15 (-1.234E+15) YES
-1.23456789E-15 rounded up to 18 precision is -1.234E-15 (-1.234E-15) YES
1.23456789E+135 rounded up to -132 precision is 1.235E+135 (1.235E+135) YES
1.23456789E-135 rounded up to 138 precision is 1.235E-135 (1.235E-135) YES
-1.23456789E+135 rounded up to -132 precision is -1.234E+135 (-1.234E+135) YES
-1.23456789E-135 rounded up to 138 precision is -1.234E-135 (-1.234E-135) YES

===Done===
