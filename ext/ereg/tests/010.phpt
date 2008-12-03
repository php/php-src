--TEST--
Long back references
--FILE--
<?php $a="abc122222222223";
  echo ereg_replace("1(2*)3","\\1def\\1",$a)?>
--EXPECT--
abc2222222222def2222222222
