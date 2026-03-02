--TEST--
Testing mb_ereg() named subpatterns
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
function_exists('mb_ereg') or die("skip mb_ereg() is not available in this build");
?>
--FILE--
<?php
    mb_regex_encoding("UTF-8");
    mb_ereg('(?<wsp>\s*)(?<word>\w+)', '  中国', $m);
    var_dump($m);
    mb_ereg('(?<wsp>\s*)(?<word>\w+)', '国', $m);
    var_dump($m);
    mb_ereg('(\s*)(?<word>\w+)', '  中国', $m);
    var_dump($m);
?>
--EXPECT--
array(5) {
  [0]=>
  string(8) "  中国"
  [1]=>
  string(2) "  "
  [2]=>
  string(6) "中国"
  ["wsp"]=>
  string(2) "  "
  ["word"]=>
  string(6) "中国"
}
array(5) {
  [0]=>
  string(3) "国"
  [1]=>
  bool(false)
  [2]=>
  string(3) "国"
  ["wsp"]=>
  bool(false)
  ["word"]=>
  string(3) "国"
}
array(3) {
  [0]=>
  string(8) "  中国"
  [1]=>
  string(6) "中国"
  ["word"]=>
  string(6) "中国"
}
