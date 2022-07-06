--TEST--
mb_ereg_replace() compat test 11
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_ereg_replace') or die("skip mb_ereg_replace() is not available in this build");
?>
--FILE--
<?php
/* (counterpart: ext/standard/tests/reg/014.phpt) */
  $a="a\\2bxc";
  echo mb_ereg_replace("a(.*)b(.*)c","\\1",$a);
?>
--EXPECT--
\2
