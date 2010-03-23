--TEST--
\0 back reference
--FILE--
<?php $a="abc123";
  echo ereg_replace("123","def\\0ghi",$a)?>
--EXPECTF--
Deprecated: Function ereg_replace() is deprecated in %s on line %d
abcdef123ghi
