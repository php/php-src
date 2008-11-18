--TEST--
RegReplace test 1
--FILE--
<?php $a="abc123";
  echo ereg_replace("123","def",$a)?>
--EXPECTF--
Deprecated: Function ereg_replace() is deprecated in %s on line %d
abcdef
