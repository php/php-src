--TEST--
mb_ereg_replace() compatibility test 7 (counterpart: ext/standard/tests/reg/010.phpt)
--POST--
--GET--
--FILE--
<?php $a="abc122222222223";
  echo mb_ereg_replace("1(2*)3","\\1def\\1",$a)?>
--EXPECT--
abc2222222222def2222222222
