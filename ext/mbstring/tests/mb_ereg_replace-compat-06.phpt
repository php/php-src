--TEST--
mb_ereg_replace() compat test 6
--SKIPIF--
extension_loaded('mbstring') or die('skip');
--FILE--
<?php
/* (counterpart: ext/standard/tests/reg/008.phpt) */
  echo mb_ereg_replace("([a-z]*)([-=+|]*)([0-9]+)","\\3 \\1 \\2\n","abc+-|=123");
?>
--EXPECT--
123 abc +-|=
