--TEST--
mb_ereg_replace() compat test 13
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_ereg_replace') or die("SKIP mb_ereg_replace() is not availab
le in this build");
?>
--FILE--
<?php
/* (counterpart: ext/standard/tests/reg/016.phpt) */
  echo mb_ereg_replace('\?',"abc","?123?");
?>
--EXPECT--
abc123abc
