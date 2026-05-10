--TEST--
GHSA-wm6j-2649-pv75
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
if (!function_exists('mb_regex_encoding')) die('skip mb_regex_encoding() not available');
?>
--FILE--
<?php
try {
	mb_regex_encoding('iso-8859-11');
} catch (ValueError $v) {
	var_dump($v->getMessage());
}
$test_str = 'x';

if (mb_ereg_search_init($test_str)) {
    $val = mb_ereg_search_pos("x");
    var_dump($val);
} else {
    var_dump(false);
}
?>
--EXPECTF--
string(%d) "mb_regex_encoding(): Argument #1 ($encoding) must be a valid encoding, "iso-8859-11" given"
array(2) {
  [0]=>
  int(0)
  [1]=>
  int(1)
}
