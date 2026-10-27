--TEST--
GH-24552 (Case-insensitive substr_compare, ksort, krsort are still locale-dependent)
--SKIPIF--
<?php
if (setlocale(LC_CTYPE, "de_DE.ISO8859-1", "de_DE") === false) {
    die("skip de_DE locale not available");
}
?>
--FILE--
<?php
setlocale(LC_CTYPE, "de_DE.ISO8859-1", "de_DE");

var_dump(substr_compare("\xC4", "\xE4", 0, 1, true));
var_dump(strncasecmp("\xC4", "\xE4", 1));

$keys = ["\xE4" => 0, "\xC4" => 0];
ksort($keys, SORT_STRING | SORT_FLAG_CASE);
var_dump(array_map('bin2hex', array_keys($keys)));

$keys = ["\xC4" => 0, "\xE4" => 0];
krsort($keys, SORT_STRING | SORT_FLAG_CASE);
var_dump(array_map('bin2hex', array_keys($keys)));
?>
--EXPECT--
int(-32)
int(-32)
array(2) {
  [0]=>
  string(2) "c4"
  [1]=>
  string(2) "e4"
}
array(2) {
  [0]=>
  string(2) "e4"
  [1]=>
  string(2) "c4"
}
