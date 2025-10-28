--TEST--
SORT_REGULAR with objects containing mixed numeric/non-numeric strings is transitive
--FILE--
<?php
class Address {
    public function __construct(
        public string $streetNumber,
        public string $streetName
    ) {}
}

echo "Test 1: Objects with mixed numeric/non-numeric string properties\n";
$addresses = [
    new Address('5', 'Main St'),
    new Address('10', 'Main St'),
    new Address('3A', 'Main St'),
];
sort($addresses, SORT_REGULAR);
echo "Result:";
foreach ($addresses as $addr) {
    echo " " . $addr->streetNumber;
}
echo "\n";

echo "\nTest 2: Same objects, different order (should match Test 1)\n";
$addresses2 = [
    new Address('3A', 'Main St'),
    new Address('10', 'Main St'),
    new Address('5', 'Main St'),
];
sort($addresses2, SORT_REGULAR);
echo "Result:";
foreach ($addresses2 as $addr) {
    echo " " . $addr->streetNumber;
}
echo "\n";

echo "\nTest 3: Another permutation (should match Test 1)\n";
$addresses3 = [
    new Address('10', 'Main St'),
    new Address('5', 'Main St'),
    new Address('3A', 'Main St'),
];
sort($addresses3, SORT_REGULAR);
echo "Result:";
foreach ($addresses3 as $addr) {
    echo " " . $addr->streetNumber;
}
echo "\n";

echo "\nTest 4: array_unique() with objects\n";
$addresses_with_dupes = [
    new Address('5', 'Main St'),
    new Address('10', 'Main St'),
    new Address('10', 'Main St'),
    new Address('3A', 'Main St'),
    new Address('5', 'Main St'),
];
$unique = array_unique($addresses_with_dupes, SORT_REGULAR);
echo "Input: 5 objects (with duplicates)\n";
echo "Output: " . count($unique) . " unique objects\n";
echo "Street numbers:";
foreach ($unique as $addr) {
    echo " " . $addr->streetNumber;
}
echo "\n";
?>
--EXPECT--
Test 1: Objects with mixed numeric/non-numeric string properties
Result: 5 10 3A

Test 2: Same objects, different order (should match Test 1)
Result: 5 10 3A

Test 3: Another permutation (should match Test 1)
Result: 5 10 3A

Test 4: array_unique() with objects
Input: 5 objects (with duplicates)
Output: 3 unique objects
Street numbers: 5 10 3A
