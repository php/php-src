--TEST--
backreferences not replaced recursively
--POST--
--GET--
--FILE--
<?php $a="a\\2bxc";
  echo ereg_replace("a(.*)b(.*)c","\\1",$a)?>
--EXPECT--
\2
