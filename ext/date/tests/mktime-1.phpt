--TEST--
Check for mktime with out-of-range parameters
--INI--
error_reporting=E_ALL
--FILE--
<?php 

date_default_timezone_set("Europe/Amsterdam");
# MacOS/X libc implementation doesn't treat out-of-range values
# the same way other unices do (Bug# 10686) so some extra code
# was added to datetime.c to take care of this 
echo date("Y-m-d", mktime( 12, 0, 0, 3,  0, 2000)) ."\n";
echo date("Y-m-d", mktime( 12, 0, 0, 3, -1, 2000)) ."\n";
echo date("Y-m-d", mktime( 12, 0, 0, 2, 29, 2000)) ."\n";
echo date("Y-m-d", mktime( 12, 0, 0, 3,  0, 2001)) ."\n";
echo date("Y-m-d", mktime( 12, 0, 0, 2, 29, 2001)) ."\n";
echo date("Y-m-d", mktime( 12, 0, 0, 0,  0, 2000)) ."\n";

?>
--EXPECT--
2000-02-29
2000-02-28
2000-02-29
2001-02-28
2001-03-01
1999-11-30
