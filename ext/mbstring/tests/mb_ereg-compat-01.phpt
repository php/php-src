--TEST--
mb_ereg() compat test 1
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
function_exists('mb_ereg') or die("skip mb_ereg() is not available in this build");
?>
--FILE--
<?php
/* (counterpart: ext/standard/tests/reg/004.phpt) */
  $a="This is a nice and simple string";
  if (mb_ereg(".*nice and simple.*",$a)) {
    echo "ok\n";
  }
  if (!mb_ereg(".*doesn't exist.*",$a)) {
    echo "ok\n";
  }
?>
--EXPECT--
ok
ok
