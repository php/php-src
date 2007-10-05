--TEST--
\0 back reference
--FILE--
<?php $a="abc123";
  echo ereg_replace("123","def\\0ghi",$a)?>
--EXPECT--
abcdef123ghi
