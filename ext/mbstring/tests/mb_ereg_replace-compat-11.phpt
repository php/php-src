--TEST--
mb_ereg_replace() compatibility test 11 (counterpart: ext/standard/tests/reg/014.phpt)
--POST--
--GET--
--FILE--
<?php $a="a\\2bxc";
  echo mb_ereg_replace("a(.*)b(.*)c","\\1",$a)?>
--EXPECT--
\2
