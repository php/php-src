--TEST--
mb_ereg_replace() compat test 10
--SKIPIF--
extension_loaded('mbstring') or die('skip');
--FILE--
<?php
/* (counterpart: ext/standard/tests/reg/013.phpt) */
  $a="abc123";
  echo mb_ereg_replace("123","def\\g\\\\hi\\",$a);
?>
--EXPECT--
abcdef\g\\hi\
