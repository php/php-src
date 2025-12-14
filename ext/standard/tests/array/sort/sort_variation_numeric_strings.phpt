--TEST--
Test sort() function: SORT_REGULAR with numeric string edge cases
--FILE--
<?php
echo "*** Testing sort() : SORT_REGULAR with numeric edge cases ***\n";

echo "\n-- Test 1: Empty string and zero variations --\n";
$a = ["", "0", "00", "A"];
sort($a, SORT_REGULAR);
var_dump($a);

echo "\n-- Test 2: Numeric strings with whitespace and signs --\n";
$a = [" 5", "+5", "-0", "0", "A"];
sort($a, SORT_REGULAR);
var_dump($a);

echo "\n-- Test 3: Scientific notation and special floats --\n";
$a = ["5e2", "500", "NAN", "INF", "-INF"];
sort($a, SORT_REGULAR);
var_dump($a);

echo "\n-- Test 4: Hexadecimal, binary and decimal strings --\n";
$a = ["0x10", "16", "0b10000"];
sort($a, SORT_REGULAR);
var_dump($a);

echo "\n-- Test 5: Mixed integers and numeric strings --\n";
$a = [10, "3A", 5, "10", ""];
sort($a, SORT_REGULAR);
var_dump($a);

echo "\n-- Test 6: LONG_MAX boundary --\n";
$a = ["9223372036854775807", "9223372036854775808", 9223372036854775807];
sort($a, SORT_REGULAR);
var_dump($a);

echo "\n-- Test 7: Leading/trailing whitespace --\n";
$a = ["5", " 5", "5 ", " 5 ", "A"];
sort($a, SORT_REGULAR);
var_dump($a);

echo "\n-- Test 8: Zero variations with signs --\n";
$a = ["0", "-0", "+0", "0.0", "-0.0"];
sort($a, SORT_REGULAR);
var_dump($a);

echo "\n-- Test 9: Multiple plus/minus signs --\n";
$a = ["++5", "--5", "+-5", "-+5", "5"];
sort($a, SORT_REGULAR);
var_dump($a);

echo "\n-- Test 10: Decimal point variations --\n";
$a = ["0.", ".0", "0.0", ".", "A"];
sort($a, SORT_REGULAR);
var_dump($a);

echo "\n-- Test 11: Leading zeros with different values --\n";
$a = ["00", "01", "001", "0", "1"];
sort($a, SORT_REGULAR);
var_dump($a);

echo "\n-- Test 12: Scientific notation variations --\n";
$a = ["1e2", "1E2", "1e+2", "1e-2", "100"];
sort($a, SORT_REGULAR);
var_dump($a);

echo "\n-- Test 13: NaN and INF float values (totalOrder) --\n";
$a = [3.5, NAN, 1, "apple", NAN, 2.0, "10", -INF, INF];
sort($a, SORT_REGULAR);
var_dump($a);

echo "\n-- Test 14: NaN and INF float values reversed --\n";
$a = [3.5, NAN, 1, "apple", NAN, 2.0, "10", -INF, INF];
rsort($a, SORT_REGULAR);
var_dump($a);

echo "\n-- Test 15: Consistency check --\n";
$results = [];
for ($i = 0; $i < 3; $i++) {
    $a = ["", "0", "00", "A"];
    sort($a, SORT_REGULAR);
    $results[] = implode(",", $a);
}
echo "All runs produce same result: " . (count(array_unique($results)) === 1 ? "yes" : "no") . "\n";

echo "Done\n";
?>
--EXPECTF--
*** Testing sort() : SORT_REGULAR with numeric edge cases ***

-- Test 1: Empty string and zero variations --
array(4) {
  [0]=>
  string(0) ""
  [1]=>
  string(1) "0"
  [2]=>
  string(2) "00"
  [3]=>
  string(1) "A"
}

-- Test 2: Numeric strings with whitespace and signs --
array(5) {
  [0]=>
  string(2) "-0"
  [1]=>
  string(1) "0"
  [2]=>
  string(2) " 5"
  [3]=>
  string(2) "+5"
  [4]=>
  string(1) "A"
}

-- Test 3: Scientific notation and special floats --
array(5) {
  [0]=>
  string(3) "5e2"
  [1]=>
  string(3) "500"
  [2]=>
  string(4) "-INF"
  [3]=>
  string(3) "INF"
  [4]=>
  string(3) "NAN"
}

-- Test 4: Hexadecimal, binary and decimal strings --
array(3) {
  [0]=>
  string(2) "16"
  [1]=>
  string(7) "0b10000"
  [2]=>
  string(4) "0x10"
}

-- Test 5: Mixed integers and numeric strings --
array(5) {
  [0]=>
  string(0) ""
  [1]=>
  int(5)
  [2]=>
  int(10)
  [3]=>
  string(2) "10"
  [4]=>
  string(2) "3A"
}

-- Test 6: LONG_MAX boundary --
array(3) {
  [0]=>
  string(19) "9223372036854775807"
  [1]=>
  string(19) "9223372036854775808"
  [2]=>
  %r(int\(9223372036854775807\)|float\(9\.22337203685477[0-9]E\+18\))%r
}

-- Test 7: Leading/trailing whitespace --
array(5) {
  [0]=>
  string(1) "5"
  [1]=>
  string(2) " 5"
  [2]=>
  string(2) "5 "
  [3]=>
  string(3) " 5 "
  [4]=>
  string(1) "A"
}

-- Test 8: Zero variations with signs --
array(5) {
  [0]=>
  string(1) "0"
  [1]=>
  string(2) "-0"
  [2]=>
  string(2) "+0"
  [3]=>
  string(3) "0.0"
  [4]=>
  string(4) "-0.0"
}

-- Test 9: Multiple plus/minus signs --
array(5) {
  [0]=>
  string(1) "5"
  [1]=>
  string(3) "++5"
  [2]=>
  string(3) "+-5"
  [3]=>
  string(3) "-+5"
  [4]=>
  string(3) "--5"
}

-- Test 10: Decimal point variations --
array(5) {
  [0]=>
  string(2) "0."
  [1]=>
  string(2) ".0"
  [2]=>
  string(3) "0.0"
  [3]=>
  string(1) "."
  [4]=>
  string(1) "A"
}

-- Test 11: Leading zeros with different values --
array(5) {
  [0]=>
  string(2) "00"
  [1]=>
  string(1) "0"
  [2]=>
  string(2) "01"
  [3]=>
  string(3) "001"
  [4]=>
  string(1) "1"
}

-- Test 12: Scientific notation variations --
array(5) {
  [0]=>
  string(4) "1e-2"
  [1]=>
  string(3) "1e2"
  [2]=>
  string(3) "1E2"
  [3]=>
  string(4) "1e+2"
  [4]=>
  string(3) "100"
}

-- Test 13: NaN and INF float values (totalOrder) --
array(9) {
  [0]=>
  float(-INF)
  [1]=>
  int(1)
  [2]=>
  float(2)
  [3]=>
  float(3.5)
  [4]=>
  float(INF)
  [5]=>
  float(NAN)
  [6]=>
  float(NAN)
  [7]=>
  string(2) "10"
  [8]=>
  string(5) "apple"
}

-- Test 14: NaN and INF float values reversed --
array(9) {
  [0]=>
  string(5) "apple"
  [1]=>
  string(2) "10"
  [2]=>
  float(NAN)
  [3]=>
  float(NAN)
  [4]=>
  float(INF)
  [5]=>
  float(3.5)
  [6]=>
  float(2)
  [7]=>
  int(1)
  [8]=>
  float(-INF)
}

-- Test 15: Consistency check --
All runs produce same result: yes
Done
