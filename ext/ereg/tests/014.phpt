--TEST--
backreferences not replaced recursively
--FILE--
<?php $a="a\\2bxc";
  echo ereg_replace("a(.*)b(.*)c","\\1",$a)?>
--EXPECTF--
Deprecated: Function ereg_replace() is deprecated in %s on line %d
\2
