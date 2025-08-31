--TEST--
Bug #32555 (strtotime("tomorrow") can return false)
--INI--
date.timezone=US/Eastern
--FILE--
<?php
$stamp = 1112427000;
print date('r', strtotime('now',$stamp)) ."\n";
print date('r', strtotime('tomorrow',$stamp)) ."\n";
print date('r', strtotime('+1 day',$stamp)) ."\n";
print date('r', strtotime('+2 day',$stamp)) ."\n";
?>
--EXPECT--
Sat, 02 Apr 2005 02:30:00 -0500
Sun, 03 Apr 2005 00:00:00 -0500
Sun, 03 Apr 2005 03:30:00 -0400
Mon, 04 Apr 2005 02:30:00 -0400
