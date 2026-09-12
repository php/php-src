--TEST--
array_str_contains() function - basic and edge cases
--FILE--
<?php

$haystack = ["apple", "banana", "cherry", "date", "elderberry"];

// 1. Basic matching
var_dump(array_str_contains($haystack, "an"));
var_dump(array_str_contains($haystack, "berry"));
var_dump(array_str_contains($haystack, "xyz"));

// 2. Preserves array keys
$assoc = ["a" => "item_one", "b" => "item_two", "c" => "other"];
var_dump(array_str_contains($assoc, "item"));

// 3. Empty needle matches everything
var_dump(array_str_contains(["foo", "bar"], ""));

// 4. Empty array returns empty array
var_dump(array_str_contains([], "test"));

?>
--EXPECT--
array(1) {
  [1]=>
  string(6) "banana"
}
array(1) {
  [4]=>
  string(10) "elderberry"
}
array(0) {
}
array(2) {
  ["a"]=>
  string(8) "item_one"
  ["b"]=>
  string(8) "item_two"
}
array(2) {
  [0]=>
  string(3) "foo"
  [1]=>
  string(3) "bar"
}
array(0) {
}
