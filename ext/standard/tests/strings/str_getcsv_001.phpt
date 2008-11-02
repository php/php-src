--TEST--
str_getcsv(): Testing using various arguments
--FILE--
<?php

// unicode input[, unicode delimiter[, unicode enclosure[, unicode escape]]]
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
var_dump(str_getcsv((binary)'1foo1 1bar111', (binary)'   ', (binary)'1   ', (binary) '\  '));
print "-----\n";
var_dump(str_getcsv('.foo  . .  bar  .', ' ', '.', ''));
print "-----\n";
var_dump(str_getcsv('" "" "', ' '));
print "-----\n";
var_dump(str_getcsv(NULL));
print "-----\n";
var_dump(str_getcsv(''));
print "-----\n";

?>
--EXPECT--
array(3) {
  [0]=>
  unicode(1) "f"
  [1]=>
  unicode(1) "o"
  [2]=>
  unicode(0) ""
}
-----
array(3) {
  [0]=>
  unicode(3) "foo"
  [1]=>
  unicode(0) ""
  [2]=>
  unicode(3) "bar"
}
-----
array(2) {
  [0]=>
  unicode(3) "foo"
  [1]=>
  unicode(3) "bar"
}
-----
array(2) {
  [0]=>
  unicode(3) "foo"
  [1]=>
  unicode(3) "bar"
}
-----
array(3) {
  [0]=>
  unicode(2) "f."
  [1]=>
  unicode(3) "bar"
  [2]=>
  unicode(4) "-|-."
}
-----
array(1) {
  [0]=>
  unicode(7) "foo.bar"
}
-----
array(2) {
  [0]=>
  unicode(3) "foo"
  [1]=>
  unicode(3) "bar"
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
  unicode(5) "foo  "
  [1]=>
  unicode(7) "  bar  "
}
-----
array(1) {
  [0]=>
  unicode(3) " " "
}
-----
array(1) {
  [0]=>
  unicode(0) ""
}
-----
array(1) {
  [0]=>
  unicode(0) ""
}
-----
