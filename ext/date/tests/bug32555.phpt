--TEST--
Bug #32555 (strtotime("tomorrow") can return false)
--FILE--
<?php
putenv("TZ=US/Eastern");

$stamp = 1112427000;
print strftime('%c %Z',strtotime('now',$stamp)) ."\n";
print strftime('%c %Z',strtotime('tomorrow',$stamp)) ."\n";
?>
--EXPECT--
Sat Apr  2 02:30:00 2005 EST
Sun Apr  3 00:00:00 2005 EST
