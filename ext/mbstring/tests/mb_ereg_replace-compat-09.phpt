--TEST--
mb_ereg_replace() compat test 9
--SKIPIF--
extension_loaded('mbstring') or die('skip');
function_exists('mb_ereg_replace') or die("SKIP mb_ereg_replace() is not available in this build");
--FILE--
<?php
/* (counterpart: ext/standard/tests/reg/012.phpt) */
  $a="abc123";
  echo mb_ereg_replace("123",'def\1ghi',$a);
?>
--EXPECT--
abcdef\1ghi
