--TEST--
mb_ereg_replace() compat test 11
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
function_exists('mb_ereg_replace') or die("skip mb_ereg_replace() is not available in this build");
?>
--FILE--
<?php
/* (counterpart: ext/standard/tests/reg/014.phpt) */
  $a="a\\2bxc";
  echo mb_ereg_replace("a(.*)b(.*)c","\\1",$a);
?>
--EXPECTF--
Deprecated: Function mb_ereg_replace() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
\2
