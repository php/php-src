--TEST--
mb_ereg_replace() compat test 11
--SKIPIF--
extension_loaded('mbstring') or die('skip');
--FILE--
<?php
/* (counterpart: ext/standard/tests/reg/014.phpt) */
  $a="a\\2bxc";
  echo mb_ereg_replace("a(.*)b(.*)c","\\1",$a);
?>
--EXPECT--
\2
