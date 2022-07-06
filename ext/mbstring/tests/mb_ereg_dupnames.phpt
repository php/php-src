--TEST--
Testing mb_ereg() duplicate named groups
--SKIPIF--
<?php
if (!extension_loaded('mbstring')) die('skip mbstring not enabled');
function_exists('mb_ereg') or die("skip mb_ereg() is not available in this build");
?>
--FILE--
<?php
    mb_regex_encoding("UTF-8");
    $pattern = '\w+((?<punct>？)|(?<punct>！))';
    mb_ereg($pattern, '中？', $m);
    var_dump($m);
    mb_ereg($pattern, '中！', $m);
    var_dump($m);
?>
--EXPECT--
array(4) {
  [0]=>
  string(6) "中？"
  [1]=>
  string(3) "？"
  [2]=>
  bool(false)
  ["punct"]=>
  string(3) "？"
}
array(4) {
  [0]=>
  string(6) "中！"
  [1]=>
  bool(false)
  [2]=>
  string(3) "！"
  ["punct"]=>
  string(3) "！"
}
