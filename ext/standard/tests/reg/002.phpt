--TEST--
RegReplace test 2
--POST--
--GET--
--FILE--
<?php $a="abc123";
  echo ereg_replace("123","",$a)?>
--EXPECT--
abc
