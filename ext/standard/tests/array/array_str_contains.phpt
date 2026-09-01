--TEST--
array_str_contains() basic functionality and edge cases
--FILE--
<?php

$fruits = ["apple", "banana", "cherry", "date", "elderberry"];

echo "--- Basic Search ---\n";
var_dump(array_str_contains($fruits, "an"));
var_dump(array_str_contains($fruits, "berry"));
var_dump(array_str_contains($fruits, "xyz"));

echo "\n--- Key Preservation ---\n";
$assoc = ["a" => "first item", "b" => "second item", "c" => "third"];
var_dump(array_str_contains($assoc, "item"));

echo "\n--- Empty Needle ---\n";
$emptyNeedleTest = ["foo", "bar", "baz"];
var_dump(array_str_contains($emptyNeedleTest, ""));

echo "\n--- UTF-8 and Multi-byte Support ---\n";
$multibyte = [
    "سلام دنیا",
    "خداحافظ دنیا",
    "PHP 8.7",
    "PHP 8.7 café 🚀",
    "یک ایموجی دیگر 🚀",
];
var_dump(array_str_contains($multibyte, "دنیا"));
var_dump(array_str_contains($multibyte, "🚀"));

echo "\n--- Empty Haystack ---\n";
var_dump(array_str_contains([], "needle"));

?>
--EXPECT--
--- Basic Search ---
array(2) {
  [1]=>
  string(6) "banana"
  [4]=>
  string(10) "elderberry"
}
array(1) {
  [4]=>
  string(10) "elderberry"
}
array(0) {
}

--- Key Preservation ---
array(2) {
  ["a"]=>
  string(10) "first item"
  ["b"]=>
  string(11) "second item"
}

--- Empty Needle ---
array(3) {
  [0]=>
  string(3) "foo"
  [1]=>
  string(3) "bar"
  [2]=>
  string(3) "baz"
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
  string(18) "PHP 8.7 café 🚀"
  [4]=>
  string(31) "یک ایموجی دیگر 🚀"
}

--- Empty Haystack ---
array(0) {
}
