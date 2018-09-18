--TEST--
Testing mb_ereg_search() named capture groups
--SKIPIF--
<?php
if (!extension_loaded('mbstring')) die('skip mbstring not enabled');
function_exists('mb_ereg_search') or die("skip mb_ereg_search() is not available in this build");
?>
--FILE--
<?php
    mb_regex_encoding("UTF-8");
    mb_ereg_search_init('  中国？');
    mb_ereg_search('(?<wsp>\s*)(?<word>\w+)(?<punct>[？！])');
    var_dump(mb_ereg_search_getregs());
?>
--EXPECT--
array(7) {
  [0]=>
  string(11) "  中国？"
  [1]=>
  string(2) "  "
  [2]=>
  string(6) "中国"
  [3]=>
  string(3) "？"
  ["punct"]=>
  string(3) "？"
  ["wsp"]=>
  string(2) "  "
  ["word"]=>
  string(6) "中国"
}
