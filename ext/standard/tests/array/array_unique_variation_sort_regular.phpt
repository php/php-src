--TEST--
Test array_unique() function : SORT_REGULAR type coercion behavior
--FILE--
<?php
echo "*** Testing array_unique() with SORT_REGULAR ***\n";

// Test 1: Integer and string representations (coerce)
echo "\n-- Integer and string coercion --\n";
var_dump(array_unique([1, "1", 2, "2"], SORT_REGULAR));

// Test 2: Boolean coercion
echo "\n-- Boolean coercion --\n";
var_dump(array_unique([true, 1, false, 0], SORT_REGULAR));

// Test 3: NULL coercion with empty string and "0"
echo "\n-- NULL coercion --\n";
var_dump(array_unique([null, "", false, 0, "0"], SORT_REGULAR));

// Test 4: Float coercion
echo "\n-- Float coercion --\n";
var_dump(array_unique([1, 1.0, "1", "1.0"], SORT_REGULAR));

// Test 5: Numeric strings coerce
echo "\n-- Numeric strings --\n";
var_dump(array_unique(["10", 10, "10.0", 10.0], SORT_REGULAR));

// Test 6: Leading zeros make strings distinct
echo "\n-- Leading zeros --\n";
var_dump(array_unique(["05", "5", 5], SORT_REGULAR));

// Test 7: Partial numeric strings don't coerce
echo "\n-- Partial numeric strings --\n";
var_dump(array_unique(["5abc", "5", 5], SORT_REGULAR));

// Test 8: Whitespace in numeric strings
echo "\n-- Whitespace in numeric strings --\n";
var_dump(array_unique(["5", " 5", "5 ", 5], SORT_REGULAR));

// Test 9: Case sensitivity for non-numeric strings
echo "\n-- Case sensitivity --\n";
var_dump(array_unique(["abc", "ABC", "Abc"], SORT_REGULAR));

// Test 10: Exponential notation coerces
echo "\n-- Exponential notation --\n";
var_dump(array_unique([1000, "1e3", "1000", 1e3], SORT_REGULAR));

// Test 11: Negative numbers
echo "\n-- Negative numbers --\n";
var_dump(array_unique([-5, "-5", -5.0], SORT_REGULAR));

// Test 12: Arrays as values
echo "\n-- Arrays --\n";
var_dump(array_unique([[1, 2], [1, 2], [1, 3]], SORT_REGULAR));

// Test 13: NaN handling (NaN != NaN)
echo "\n-- NaN handling --\n";
var_dump(array_unique([NAN, NAN, 1], SORT_REGULAR));

// Test 14: INF handling
echo "\n-- INF handling --\n";
var_dump(array_unique([INF, INF, -INF, -INF], SORT_REGULAR));

// Test 15: Bug GH-20262 - mixed numeric and alphanumeric
echo "\n-- Bug GH-20262 case --\n";
var_dump(array_unique(['5', '10', '3A', '5'], SORT_REGULAR));

// Test 16: SORT_REGULAR vs SORT_STRING comparison
echo "\n-- SORT_REGULAR vs SORT_STRING --\n";
$input = [true, 1, "1"];
echo "SORT_REGULAR: ";
var_dump(array_unique($input, SORT_REGULAR));
echo "SORT_STRING: ";
var_dump(array_unique($input, SORT_STRING));

echo "\nDone\n";
?>
--EXPECT--
*** Testing array_unique() with SORT_REGULAR ***

-- Integer and string coercion --
array(2) {
  [0]=>
  int(1)
  [2]=>
  int(2)
}

-- Boolean coercion --
array(2) {
  [0]=>
  bool(true)
  [2]=>
  bool(false)
}

-- NULL coercion --
array(2) {
  [0]=>
  NULL
  [4]=>
  string(1) "0"
}

-- Float coercion --
array(1) {
  [0]=>
  int(1)
}

-- Numeric strings --
array(1) {
  [0]=>
  string(2) "10"
}

-- Leading zeros --
array(1) {
  [0]=>
  string(2) "05"
}

-- Partial numeric strings --
array(2) {
  [0]=>
  string(4) "5abc"
  [1]=>
  string(1) "5"
}

-- Whitespace in numeric strings --
array(1) {
  [0]=>
  string(1) "5"
}

-- Case sensitivity --
array(3) {
  [0]=>
  string(3) "abc"
  [1]=>
  string(3) "ABC"
  [2]=>
  string(3) "Abc"
}

-- Exponential notation --
array(1) {
  [0]=>
  int(1000)
}

-- Negative numbers --
array(1) {
  [0]=>
  int(-5)
}

-- Arrays --
array(2) {
  [0]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(2)
  }
  [2]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(3)
  }
}

-- NaN handling --
array(3) {
  [0]=>
  float(NAN)
  [1]=>
  float(NAN)
  [2]=>
  int(1)
}

-- INF handling --
array(2) {
  [0]=>
  float(INF)
  [2]=>
  float(-INF)
}

-- Bug GH-20262 case --
array(3) {
  [0]=>
  string(1) "5"
  [1]=>
  string(2) "10"
  [2]=>
  string(2) "3A"
}

-- SORT_REGULAR vs SORT_STRING --
SORT_REGULAR: array(1) {
  [0]=>
  bool(true)
}
SORT_STRING: array(1) {
  [0]=>
  bool(true)
}

Done
