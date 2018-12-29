--TEST--
Bug #77367 (Negative size parameter in mb_split)
--SKIPIF--
<?php
if (!extension_loaded('mbstring')) die('mbstring extension not available');
if (!function_exists('mb_split')) die('mb_split() not available');
?>
--FILE--
<?php
mb_regex_encoding('UTF-8');
var_dump(mb_split("\\w", "\xfc"));
?>
===DONE===
--EXPECT--
array(2) {
  [0]=>
  string(0) ""
  [1]=>
  string(0) ""
}
===DONE===
