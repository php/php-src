--TEST--
mb_ereg_replace() compat test 13
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
function_exists('mb_ereg_replace') or die("skip mb_ereg_replace() is not available in this build");
?>
--FILE--
<?php
/* (counterpart: ext/standard/tests/reg/016.phpt) */
  echo mb_ereg_replace('\?',"abc","?123?");
?>
--EXPECT--
abc123abc
