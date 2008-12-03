--TEST--
ereg_replace single-quote test
--FILE--
<?php $a="\\'test";
  echo ereg_replace("\\\\'","'",$a)
?>
--EXPECT--
'test
