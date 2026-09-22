--TEST--
mb_ereg_replace() compat test 10
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
function_exists('mb_ereg_replace') or die("skip mb_ereg_replace() is not available in this build");
?>
--FILE--
<?php
/* (counterpart: ext/standard/tests/reg/013.phpt) */
  $a="abc123";
  echo mb_ereg_replace("123","def\\g\\\\hi\\",$a);
?>
--EXPECTF--
Deprecated: Function mb_ereg_replace() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
abcdef\g\\hi\
