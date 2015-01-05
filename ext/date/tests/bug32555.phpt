--TEST--
Bug #32555 (strtotime("tomorrow") can return false)
--SKIPIF--
<?php 
if (substr(PHP_OS, 0, 3) == 'WIN') die('skip strftime uses system TZ'); 
?>
--INI--
date.timezone=US/Eastern
--FILE--
<?php
$stamp = 1112427000;
print strftime('%c %Z',strtotime('now',$stamp)) ."\n";
print strftime('%c %Z',strtotime('tomorrow',$stamp)) ."\n";
print strftime('%c %Z',strtotime('+1 day',$stamp)) ."\n";
print strftime('%c %Z',strtotime('+2 day',$stamp)) ."\n";
?>
--EXPECT--
Sat Apr  2 02:30:00 2005 EST
Sun Apr  3 00:00:00 2005 EST
Sun Apr  3 03:30:00 2005 EDT
Mon Apr  4 02:30:00 2005 EDT
