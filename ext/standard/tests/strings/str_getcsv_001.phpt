--TEST--
str_getcsv(): Testing using various arguments
--FILE--
<?php

// string input[, string delimiter[, string enclosure[, string escape]]]
var_dump(str_getcsv('"f", "o", ""'));
print "-----\n";
var_dump(str_getcsv('foo||bar', '|'));
print "-----\n";
var_dump(str_getcsv('foo|bar', '|'));
print "-----\n";
var_dump(str_getcsv('|foo|-|bar|', '-', '|'));
print "-----\n";
var_dump(str_getcsv('|f.|.|bar|.|-|-.|', '.', '|', '-'));
print "-----\n";
var_dump(str_getcsv('.foo..bar.', '.', '.', '.'));
print "-----\n";
var_dump(str_getcsv('.foo  . .  bar  .', ' ', '.', ''));
print "-----\n";
var_dump(str_getcsv('" "" "', ' '));
print "-----\n";
var_dump(str_getcsv(''));
print "-----\n";

?>
--EXPECTF--
array(3) {
  [0]=>
  string(1) "f"
  [1]=>
  string(1) "o"
  [2]=>
  string(0) ""
}
-----
array(3) {
  [0]=>
  string(3) "foo"
  [1]=>
  string(0) ""
  [2]=>
  string(3) "bar"
}
-----
array(2) {
  [0]=>
  string(3) "foo"
  [1]=>
  string(3) "bar"
}
-----
array(2) {
  [0]=>
  string(3) "foo"
  [1]=>
  string(3) "bar"
}
-----

Deprecated: str_getcsv(): Passing a non-empty string to the $escape parameter is deprecated since 8.4 in %s on line %d
array(3) {
  [0]=>
  string(2) "f."
  [1]=>
  string(3) "bar"
  [2]=>
  string(4) "-|-."
}
-----

Deprecated: str_getcsv(): Passing a non-empty string to the $escape parameter is deprecated since 8.4 in %s on line %d
array(1) {
  [0]=>
  string(7) "foo.bar"
}
-----
array(2) {
  [0]=>
  string(5) "foo  "
  [1]=>
  string(7) "  bar  "
}
-----
array(1) {
  [0]=>
  string(3) " " "
}
-----
array(1) {
  [0]=>
  NULL
}
-----
