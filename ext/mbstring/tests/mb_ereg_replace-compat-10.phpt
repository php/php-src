--TEST--
mb_ereg_replace() compatibility test 10 (counterpart: ext/standard/tests/reg/013.phpt)
--POST--
--GET--
--FILE--
<?php $a="abc123";
  echo mb_ereg_replace("123","def\\g\\\\hi\\",$a)?>
--EXPECT--
abcdef\g\\hi\
