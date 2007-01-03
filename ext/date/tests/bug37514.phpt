--TEST--
Bug #37514 (strtotime doesn't assume year correctly).
--INI--
date.timezone=UTC
--FILE--
<?php
echo date('r', strtotime('May 18th 5:05')), "\n";
echo date('r', strtotime('May 18th 5:05pm')), "\n";
echo date('r', strtotime('May 18th 5:05 pm')), "\n";
echo date('r', strtotime('May 18th 5:05am')), "\n";
echo date('r', strtotime('May 18th 5:05 am')), "\n";
echo date('r', strtotime('May 18th 2006 5:05pm')), "\n";
?>
--EXPECTF--
Thu, 18 May %d 05:05:00 +0000
Thu, 18 May %d 17:05:00 +0000
Thu, 18 May %d 17:05:00 +0000
Thu, 18 May %d 05:05:00 +0000
Thu, 18 May %d 05:05:00 +0000
Thu, 18 May %d 17:05:00 +0000
