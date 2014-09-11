--TEST--
ereg_replace single-quote test
--FILE--
<?php $a="\\'test";
  echo ereg_replace("\\\\'","'",$a)
?>
--EXPECTF--
Deprecated: Function ereg_replace() is deprecated in %s on line %d
'test
