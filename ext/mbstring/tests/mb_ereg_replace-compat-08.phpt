--TEST--
mb_ereg_replace() compatibility test 8 (counterpart: ext/standard/tests/reg/011.phpt)
--POST--
--GET--
--FILE--
<?php $a="abc123";
  echo mb_ereg_replace("123","def\\0ghi",$a)?>
--EXPECT--
abcdef123ghi
