--TEST--
Bug #35425 (idate() function ignores timezone settings)
--INI--
date.timezone=America/Montreal
--FILE--
<?php

$time = mktime(1,1,1,1,1,2005);
foreach (array('B','d','h','H','i','I','L','m','s','t','U','w','W','y','Y','z','Z') as $v) {
    var_dump(idate($v, $time));
}

?>
--EXPECT--
int(292)
int(1)
int(1)
int(1)
int(1)
int(0)
int(0)
int(1)
int(1)
int(31)
int(1104559261)
int(6)
int(53)
int(5)
int(2005)
int(0)
int(-18000)
