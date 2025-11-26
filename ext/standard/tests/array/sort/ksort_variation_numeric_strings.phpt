--TEST--
Test ksort() function: SORT_REGULAR consistency with sort() for numeric string keys
--FILE--
<?php
echo "*** Testing ksort() : SORT_REGULAR consistency with sort() ***\n";

echo "\n-- Test 1: Hexadecimal, binary and decimal strings --\n";
$values = ["0x10", "16", "0b10000"];
$keyed = ["0x10" => "a", "16" => "b", "0b10000" => "c"];

sort($values, SORT_REGULAR);
ksort($keyed, SORT_REGULAR);

echo "sort() result:  ";
var_dump($values);
echo "ksort() result: ";
var_dump(array_keys($keyed));

echo "\n-- Test 2: Mixed integers and numeric strings (from sort test) --\n";
// Note: This uses actual integer keys mixed with string keys
$values = [10, "3A", 5, "10", ""];
$keyed = [10 => "a", "3A" => "b", 5 => "c", "10" => "d", "" => "e"];

sort($values, SORT_REGULAR);
ksort($keyed, SORT_REGULAR);

echo "sort() result:  ";
var_dump($values);
echo "ksort() result: ";
var_dump(array_keys($keyed));

echo "\n-- Test 3: Consistency check (multiple runs) --\n";
$results = [];
for ($i = 0; $i < 3; $i++) {
    $keyed = ["5" => 1, "3A" => 2, "10" => 3];
    ksort($keyed, SORT_REGULAR);
    $results[] = implode(",", array_keys($keyed));
}
echo "All runs produce same result: " . (count(array_unique($results)) === 1 ? "yes" : "no") . "\n";

echo "Done\n";
?>
--EXPECT--
*** Testing ksort() : SORT_REGULAR consistency with sort() ***

-- Test 1: Hexadecimal, binary and decimal strings --
sort() result:  array(3) {
  [0]=>
  string(2) "16"
  [1]=>
  string(7) "0b10000"
  [2]=>
  string(4) "0x10"
}
ksort() result: array(3) {
  [0]=>
  int(16)
  [1]=>
  string(7) "0b10000"
  [2]=>
  string(4) "0x10"
}

-- Test 2: Mixed integers and numeric strings (from sort test) --
sort() result:  array(5) {
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
ksort() result: array(4) {
  [0]=>
  string(0) ""
  [1]=>
  int(5)
  [2]=>
  int(10)
  [3]=>
  string(2) "3A"
}

-- Test 3: Consistency check (multiple runs) --
All runs produce same result: yes
Done
