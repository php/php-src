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
var_dump(str_getcsv('.foo. .bar.', '   ', '.', '.'));
print "-----\n";
var_dump(str_getcsv('1foo1 1bar111', '   ', '1   ', '\  '));
print "-----\n";
var_dump(str_getcsv('.foo  . .  bar  .', ' ', '.', ''));
print "-----\n";
var_dump(str_getcsv('" "" "', ' '));
print "-----\n";
var_dump(str_getcsv(''));
print "-----\n";

?>
--EXPECT--
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
array(3) {
  [0]=>
  string(2) "f."
  [1]=>
  string(3) "bar"
  [2]=>
  string(4) "-|-."
}
-----
array(1) {
  [0]=>
  string(7) "foo.bar"
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
  string(4) "bar1"
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
