--TEST--
array_str_contains() basic and edge cases tests
--FILE--
<?php

echo "--- Basic Search ---\n";
$fruits = ['apple', 'banana', 'cherry', 'pineapple', 'grape'];
var_dump(array_str_contains($fruits, 'apple'));

echo "\n--- Preserving String and Numeric Keys ---\n";
$data = [
    'first' => 'hello world',
    10      => 'foo bar',
    'third' => 'world domination',
    20      => 'just php',
];
var_dump(array_str_contains($data, 'world'));

echo "\n--- Non-string Values Ignored ---\n";
$mixed = [
    'a' => 'test 1',
    'b' => 123,
    'c' => null,
    'd' => 'another test',
    'e' => ['nested'],
    'f' => true,
];
var_dump(array_str_contains($mixed, 'test'));

echo "\n--- Empty Needle (Matches All String Elements) ---\n";
$items = ['one', 2, 'three', false, ''];
var_dump(array_str_contains($items, ''));

echo "\n--- UTF-8 and Multi-byte Support ---\n";
$persian = [
    'سلام دنیا',
    'خداحافظ دنیا',
    'تست پی‌اچ‌پی',
    'PHP 8.7 عالیه 🚀',
    'یک ایموجی دیگر 🚀'
];
var_dump(array_str_contains($persian, 'دنیا'));
var_dump(array_str_contains($persian, '🚀'));

echo "\n--- Empty Haystack ---\n";
var_dump(array_str_contains([], 'anything'));

?>
--EXPECT--
--- Basic Search ---
array(2) {
  [0]=>
  string(5) "apple"
  [3]=>
  string(9) "pineapple"
}

--- Preserving String and Numeric Keys ---
array(2) {
  ["first"]=>
  string(11) "hello world"
  ["third"]=>
  string(16) "world domination"
}

--- Non-string Values Ignored ---
array(2) {
  ["a"]=>
  string(6) "test 1"
  ["d"]=>
  string(12) "another test"
}

--- Empty Needle (Matches All String Elements) ---
array(3) {
  [0]=>
  string(3) "one"
  [2]=>
  string(5) "three"
  [4]=>
  string(0) ""
}

--- UTF-8 and Multi-byte Support ---
array(2) {
  [0]=>
  string(17) "سلام دنیا"
  [1]=>
  string(23) "خداحافظ دنیا"
}
array(2) {
  [3]=>
  string(23) "PHP 8.7" عالیه 🚀"
  [4]=>
  string(31) "یک ایموجی دیگر 🚀"
}

--- Empty Haystack ---
array(0) {
}
