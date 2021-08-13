--TEST--
mb_ereg_replace() compat test 3
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
function_exists('mb_ereg_replace') or die("skip mb_ereg_replace() is not available in this build");
?>
--FILE--
<?php
/* (counterpart: ext/standard/tests/reg/003.phpt) */
  $a="\\'test";
  echo mb_ereg_replace("\\\\'","'",$a);
?>
--EXPECT--
'test
