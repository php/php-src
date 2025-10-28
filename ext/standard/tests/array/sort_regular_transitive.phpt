--TEST--
SORT_REGULAR uses transitive comparison for consistent sorting
--FILE--
<?php
echo "Test 1: Scalar mixed numeric/non-numeric strings\n";
$arr1 = ["5", "10", "3A"];
sort($arr1, SORT_REGULAR);
var_dump($arr1);

echo "\nTest 2: Same values, different order (should match)\n";
$arr2 = ["3A", "10", "5"];
sort($arr2, SORT_REGULAR);
var_dump($arr2);

echo "\nTest 3: Nested arrays with mixed strings\n";
$arr3 = [
    ['streetNumber' => '5'],
    ['streetNumber' => '10'],
    ['streetNumber' => '3A'],
];
sort($arr3, SORT_REGULAR);
foreach ($arr3 as $item) {
    echo $item['streetNumber'];
    if ($item !== end($arr3)) echo " ";
}
echo "\n";

echo "\nTest 4: Same nested arrays, reversed (should match)\n";
$arr4 = [
    ['streetNumber' => '3A'],
    ['streetNumber' => '10'],
    ['streetNumber' => '5'],
];
sort($arr4, SORT_REGULAR);
foreach ($arr4 as $item) {
    echo $item['streetNumber'];
    if ($item !== end($arr4)) echo " ";
}
echo "\n";

echo "\nTest 5: array_unique with nested arrays\n";
$arr5 = [
    ['number' => '5'],
    ['number' => '10'],
    ['number' => '5'],
    ['number' => '3A'],
    ['number' => '5'],
];
$unique = array_unique($arr5, SORT_REGULAR);
echo "Unique count: " . count($unique) . "\n";

echo "\nTest 6: Comparison operators remain unchanged\n";
echo '"5" <=> "3A": ' . ("5" <=> "3A") . "\n";
echo '"10" <=> "3A": ' . ("10" <=> "3A") . "\n";
?>
--EXPECT--
Test 1: Scalar mixed numeric/non-numeric strings
array(3) {
  [0]=>
  string(1) "5"
  [1]=>
  string(2) "10"
  [2]=>
  string(2) "3A"
}

Test 2: Same values, different order (should match)
array(3) {
  [0]=>
  string(1) "5"
  [1]=>
  string(2) "10"
  [2]=>
  string(2) "3A"
}

Test 3: Nested arrays with mixed strings
5 10 3A

Test 4: Same nested arrays, reversed (should match)
5 10 3A

Test 5: array_unique with nested arrays
Unique count: 3

Test 6: Comparison operators remain unchanged
"5" <=> "3A": 1
"10" <=> "3A": -1
