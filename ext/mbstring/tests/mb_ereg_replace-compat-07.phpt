--TEST--
mb_ereg_replace() compat test 7
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
function_exists('mb_ereg_replace') or die("skip mb_ereg_replace() is not available in this build");
?>
--FILE--
<?php
/* (counterpart: ext/standard/tests/reg/010.phpt) */
  $a="abc122222222223";
  echo mb_ereg_replace("1(2*)3","\\1def\\1",$a);
?>
--EXPECT--
abc2222222222def2222222222
