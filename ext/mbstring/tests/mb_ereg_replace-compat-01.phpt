--TEST--
mb_ereg_replace() compat test 1
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
function_exists('mb_ereg_replace') or die("skip mb_ereg_replace() is not available in this build");
?>
--FILE--
<?php
/* (counterpart: ext/standard/tests/reg/001.phpt) */
  $a="abc123";
  echo mb_ereg_replace("123","def",$a);
?>
--EXPECT--
abcdef
