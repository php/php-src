--TEST--
mb_ereg_replace() compatibility test 9 (counterpart: ext/standard/tests/reg/012.phpt)
--POST--
--GET--
--FILE--
<?php $a="abc123";
  echo mb_ereg_replace("123",'def\1ghi',$a)?>
--EXPECT--
abcdef\1ghi
