--TEST--
mb_ereg_replace() compat test 13
--SKIPIF--
extension_loaded('mbstring') or die('skip');
--FILE--
<?php
/* (counterpart: ext/standard/tests/reg/016.phpt) */
  echo mb_ereg_replace('\?',"abc","?123?");
?>
--EXPECT--
abc123abc
