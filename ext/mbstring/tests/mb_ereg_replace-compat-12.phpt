--TEST--
mb_ereg_replace() compat test 12
--SKIPIF--
extension_loaded('mbstring') or die('skip');
--FILE--
<?php
/* (counterpart: ext/standard/tests/reg/015.phpt) */
  echo mb_ereg_replace("^","z","abc123");
?>
--EXPECT--
zabc123
