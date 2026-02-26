--TEST--
GH-20262 (array_unique() with SORT_REGULAR returns duplicate values)
--FILE--
<?php
echo "Test 1: Scalar array (original bug report)\n";
$units = ['5', '10', '5', '3A', '5', '5'];
$unique = array_unique($units, SORT_REGULAR);
print_r($unique);

echo "\nTest 2: Same array with SORT_STRING\n";
$unique = array_unique($units, SORT_STRING);
print_r($unique);

echo "\nTest 3: Objects\n";
class Address {
    public function __construct(
        public string $streetNumber,
        public string $streetName
    ) {}
}

$addresses = [
    new Address('5', 'Main St'),
    new Address('10', 'Main St'),
    new Address('10', 'Main St'),
    new Address('3A', 'Main St'),
    new Address('5', 'Main St'),
];

$unique = array_unique($addresses, SORT_REGULAR);
echo "Unique count: " . count($unique) . " (expected 3)\n";
echo "Street numbers:";
foreach ($unique as $addr) {
    echo " " . $addr->streetNumber;
}
echo "\n";

echo "\nTest 4: Nested arrays\n";
$addresses = [
    ['streetNumber' => '5', 'streetName' => 'Main St'],
    ['streetNumber' => '10', 'streetName' => 'Main St'],
    ['streetNumber' => '10', 'streetName' => 'Main St'],
    ['streetNumber' => '3A', 'streetName' => 'Main St'],
    ['streetNumber' => '5', 'streetName' => 'Main St'],
];

$unique = array_unique($addresses, SORT_REGULAR);
echo "Unique count: " . count($unique) . " (expected 3)\n";
echo "Street numbers:";
foreach ($unique as $addr) {
    echo " " . $addr['streetNumber'];
}
echo "\n";

echo "\nTest 5: sort() consistency with SORT_REGULAR\n";
$arr1 = ["5", "10", "3A"];
$arr2 = ["3A", "10", "5"];
sort($arr1, SORT_REGULAR);
sort($arr2, SORT_REGULAR);
echo "arr1 sorted: ['" . implode("', '", $arr1) . "']\n";
echo "arr2 sorted: ['" . implode("', '", $arr2) . "']\n";
echo "Results match: " . ($arr1 === $arr2 ? "yes" : "no") . "\n";

?>
--EXPECT--
Test 1: Scalar array (original bug report)
Array
(
    [0] => 5
    [1] => 10
    [3] => 3A
)

Test 2: Same array with SORT_STRING
Array
(
    [0] => 5
    [1] => 10
    [3] => 3A
)

Test 3: Objects
Unique count: 3 (expected 3)
Street numbers: 5 10 3A

Test 4: Nested arrays
Unique count: 3 (expected 3)
Street numbers: 5 10 3A

Test 5: sort() consistency with SORT_REGULAR
arr1 sorted: ['5', '10', '3A']
arr2 sorted: ['5', '10', '3A']
Results match: yes
