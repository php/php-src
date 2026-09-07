--TEST--
mb_split() empty match
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
function_exists('mb_split') or die("skip mb_split() is not available in this build");
?>
--FILE--
<?php
mb_regex_set_options('m');
var_dump(mb_split('^', "a\nb\nc"));
?>
--EXPECTF--
Deprecated: Function mb_regex_set_options() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d

Deprecated: Function mb_split() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
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
