--TEST--
RegReplace test 2
--FILE--
<?php $a="abc123";
  echo ereg_replace("123","",$a)?>
--EXPECTF--
Deprecated: Function ereg_replace() is deprecated in %s on line %d
abc
