--TEST--
ereg_replace single-quote test
--POST--
--GET--
--FILE--
<?php $a="\\'test";
  echo ereg_replace("\\\\'","'",$a)
?>
--EXPECT--
'test
