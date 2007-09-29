--TEST--
Test sprintf() function : usage variations - typical format strings
--FILE--
<?php
/* Prototype  : string sprintf(string $format [, mixed $arg1 [, mixed ...]])
 * Description: Return a formatted string 
 * Source code: ext/standard/formatted_print.c
*/

echo "*** Testing sprintf() : with  typical format strings ***\n";

// initialising required variables
$tempnum = 12345;
$tempstring = "abcdefghjklmnpqrstuvwxyz";

echo"\n-- Testing for '%%%.2f' as the format parameter --\n";
var_dump(sprintf("%%%.2f", 1.23456789e10));

echo"\n-- Testing for '%%' as the format parameter --\n";
var_dump(sprintf("%%", 1.23456789e10));

echo"\n-- Testing for precision value more than maximum --\n";
var_dump(sprintf("%.988f", 1.23456789e10));

echo"\n-- Testing for invalid width(-15) specifier --\n";
var_dump(sprintf("%030.-15s", $tempstring));

echo"\n-- Testing for '%X' as the format parameter --\n";
var_dump(sprintf("%X", 12));

echo"\n-- Testing for multiple format parameters --\n";
var_dump(sprintf("%d  %s  %d\n", $tempnum, $tempstring, $tempnum));

echo"\n-- Testing for excess of mixed type arguments  --\n";
var_dump(sprintf("%s", $tempstring, $tempstring, $tempstring));

echo "Done";
?>
--EXPECTF--
*** Testing sprintf() : with  typical format strings ***

-- Testing for '%%%.2f' as the format parameter --
string(15) "%12345678900.00"

-- Testing for '%%' as the format parameter --
string(1) "%"

-- Testing for precision value more than maximum --
string(52) "12345678900.0000000000000000000000000000000000000000"

-- Testing for invalid width(-15) specifier --
string(3) "15s"

-- Testing for '%X' as the format parameter --
string(1) "C"

-- Testing for multiple format parameters --
string(39) "12345  abcdefghjklmnpqrstuvwxyz  12345
"

-- Testing for excess of mixed type arguments  --
string(24) "abcdefghjklmnpqrstuvwxyz"
Done
