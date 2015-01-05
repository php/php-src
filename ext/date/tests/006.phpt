--TEST--
checkdate() tests
--FILE--
<?php
date_default_timezone_set('UTC');

var_dump(checkdate());
var_dump(checkdate(1,1,1));

var_dump(checkdate(2,29,2006));
var_dump(checkdate(31,6,2006));
var_dump(checkdate(1,1,32768));
var_dump(checkdate(1,1,32767));

var_dump(checkdate(-1,1,2006));
var_dump(checkdate(1,-1,2006));
var_dump(checkdate(1,1,-1));

echo "Done\n";
?>
--EXPECTF--	
Warning: checkdate() expects exactly 3 parameters, 0 given in %s on line %d
bool(false)
bool(true)
bool(false)
bool(false)
bool(false)
bool(true)
bool(false)
bool(false)
bool(false)
Done
