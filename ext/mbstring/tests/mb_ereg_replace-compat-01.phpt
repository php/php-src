--TEST--
mb_ereg_replace() compatibility test 1 (counterpart: ext/standard/tests/reg/001.phpt)
--POST--
--GET--
--FILE--
<?php $a="abc123";
  echo mb_ereg_replace("123","def",$a)?>
--EXPECT--
abcdef
