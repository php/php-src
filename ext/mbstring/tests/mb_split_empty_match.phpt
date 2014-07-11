--TEST--
mb_split() empty match
--
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_split') or die("skip mb_split() is not available in this build");
?>
--FILE--
<?php
mb_regex_set_options('m');
var_dump(mb_split('^', "a\nb\nc"));
--EXPECT--
array(3) {
  [0]=>
  string(2) "a
"
  [1]=>
  string(2) "b
"
  [2]=>
  string(1) "c"
}
