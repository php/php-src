--TEST--
RegReplace test 1
--POST--
--GET--
--FILE--
<?php $a="abc123";
  echo ereg_replace("123","def",$a)?>
--EXPECT--
abcdef
