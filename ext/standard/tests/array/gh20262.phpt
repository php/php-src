--TEST--
GH-20262: array_unique() with SORT_REGULAR fails to identify duplicates with mixed numeric/alphanumeric strings
--FILE--
<?php
// Original bug report: array_unique() with SORT_REGULAR doesn't properly
// identify duplicates when array contains mixed numeric and alphanumeric strings
$units = ['5', '10', '5', '3A', '5', '5'];
$unique = array_unique($units, SORT_REGULAR);

echo "Input array:\n";
var_dump($units);

echo "\nResult of array_unique():\n";
var_dump($unique);

echo "\nExpected: 3 unique values ('5', '10', '3A')\n";
echo "Actual count: " . count($unique) . "\n";

// Additional test: verify sort() groups equal values correctly
$arr = ['5', '10', '5', '3A', '5', '5'];
sort($arr);

echo "\nSorted array (equal values should be grouped):\n";
var_dump($arr);

// Verify '5' values are consecutive
$positions = [];
foreach ($arr as $idx => $val) {
    if ($val === '5') {
        $positions[] = $idx;
    }
}

$consecutive = true;
for ($i = 0; $i < count($positions) - 1; $i++) {
    if ($positions[$i] + 1 !== $positions[$i + 1]) {
        $consecutive = false;
        break;
    }
}

echo "\nAll '5' values grouped together: " . ($consecutive ? "yes" : "no") . "\n";
?>
--EXPECT--
Input array:
array(6) {
  [0]=>
  string(1) "5"
  [1]=>
  string(2) "10"
  [2]=>
  string(1) "5"
  [3]=>
  string(2) "3A"
  [4]=>
  string(1) "5"
  [5]=>
  string(1) "5"
}

Result of array_unique():
array(3) {
  [0]=>
  string(1) "5"
  [1]=>
  string(2) "10"
  [3]=>
  string(2) "3A"
}

Expected: 3 unique values ('5', '10', '3A')
Actual count: 3

Sorted array (equal values should be grouped):
array(6) {
  [0]=>
  string(2) "3A"
  [1]=>
  string(1) "5"
  [2]=>
  string(1) "5"
  [3]=>
  string(1) "5"
  [4]=>
  string(1) "5"
  [5]=>
  string(2) "10"
}

All '5' values grouped together: yes
