--TEST--
mb_ereg_replace() compat test 12
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_ereg_replace') or die("skip mb_ereg_replace() is not available in this build");
?>
--FILE--
<?php
/* (counterpart: ext/standard/tests/reg/015.phpt) */
  echo mb_ereg_replace("^","z","abc123");
?>
--EXPECT--
zabc123
