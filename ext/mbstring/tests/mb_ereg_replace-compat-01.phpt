--TEST--
mb_ereg_replace() compat test 1
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
function_exists('mb_ereg_replace') or die("skip mb_ereg_replace() is not available in this build");
?>
--FILE--
<?php
/* (counterpart: ext/standard/tests/reg/001.phpt) */
  $a="abc123";
  echo mb_ereg_replace("123","def",$a);
?>
--EXPECTF--
Deprecated: Function mb_ereg_replace() is deprecated since 8.6, mbregex support end PHP 9.0 in %smbstring/tests/mb_ereg_replace-compat-01.php on line %d
abcdef
