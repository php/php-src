--TEST--
mb_ereg_replace() compatibility test 2 (counterpart: ext/standard/tests/reg/002.phpt)
--POST--
--GET--
--FILE--
<?php $a="abc123";
  echo mb_ereg_replace("123","",$a)?>
--EXPECT--
abc
