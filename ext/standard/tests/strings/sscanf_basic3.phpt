--TEST--
Test sscanf() function : basic functionality - float format
--FILE--
<?php

echo "*** Testing sscanf() : basic functionality -- using float format ***\n";

$str = "Part: Widget Length: 111.53 Width: 22.345 Depth: 12.4";
$format = "Part: %s Length: %f Width: %f Depth: %f";

echo "\n-- Try sccanf() WITHOUT optional args --\n";
// extract details using short format
list($part, $length, $width, $depth) = sscanf($str, $format);
var_dump($part, $length, $width, $depth);

echo "\n-- Try sccanf() WITH optional args --\n";
// extract details using long  format
$res = sscanf($str, $format, $part, $length, $width, $depth);
var_dump($res, $part, $length, $width, $depth);

?>
--EXPECT--
*** Testing sscanf() : basic functionality -- using float format ***

-- Try sccanf() WITHOUT optional args --
string(6) "Widget"
float(111.53)
float(22.345)
float(12.4)

-- Try sccanf() WITH optional args --
int(4)
string(6) "Widget"
float(111.53)
float(22.345)
float(12.4)
