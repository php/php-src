--TEST--
mb_ereg_replace() compatibility test 3 (counterpart: ext/standard/tests/reg/003.phpt)
--POST--
--GET--
--FILE--
<?php $a="\\'test";
  echo mb_ereg_replace("\\\\'","'",$a)
?>
--EXPECT--
'test
