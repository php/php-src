--TEST--
mb_ereg() compatibility test 1 (counterpart: ext/standard/tests/reg/004.phpt)
--POST--
--GET--
--FILE--
<?php $a="This is a nice and simple string";
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
