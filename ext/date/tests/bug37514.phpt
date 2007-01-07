--TEST--
Bug #37514 (strtotime doesn't assume year correctly).
--INI--
date.timezone=UTC
--FILE--
<?php
echo date('r', strtotime('May 18th 5:05', 1168156376)), "\n";
echo date('r', strtotime('May 18th 5:05pm', 1168156376)), "\n";
echo date('r', strtotime('May 18th 5:05 pm', 1168156376)), "\n";
echo date('r', strtotime('May 18th 5:05am', 1168156376)), "\n";
echo date('r', strtotime('May 18th 5:05 am', 1168156376)), "\n";
echo date('r', strtotime('May 18th 2006 5:05pm', 1168156376)), "\n";
?>
--EXPECT--
Fri, 18 May 2007 05:05:00 +0000
Fri, 18 May 2007 17:05:00 +0000
Fri, 18 May 2007 17:05:00 +0000
Fri, 18 May 2007 05:05:00 +0000
Fri, 18 May 2007 05:05:00 +0000
Thu, 18 May 2006 17:05:00 +0000
