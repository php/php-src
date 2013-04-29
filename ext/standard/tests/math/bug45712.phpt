--TEST--
Bug #45712 (NaN/INF comparison)
--XFAIL--
Bug 45712 not fixed yet.
--FILE--
<?php

// NaN
$nan = acos(1.01);
var_dump($nan);
var_dump(is_nan($nan));
//
var_dump($nan=='');
var_dump($nan==0.5);
var_dump($nan==50);
var_dump($nan=='500');
var_dump($nan=='abc');
var_dump($nan==$nan);
// 
var_dump($nan==='');
var_dump($nan===0.5);
var_dump($nan===50);
var_dump($nan==='500');
var_dump($nan==='abc');
var_dump($nan===$nan);

// INF
$inf = pow(0,-2);
var_dump($inf);
var_dump(is_infinite($inf));
//
var_dump($inf=='');
var_dump($inf==0.5);
var_dump($inf==50);
var_dump($inf=='500');
var_dump($inf=='abc');
var_dump($inf==$inf);
// 
var_dump($inf==='');
var_dump($inf===0.5);
var_dump($inf===50);
var_dump($inf==='500');
var_dump($inf==='abc');
var_dump($inf===$inf);

?>
--EXPECT--
float(NAN)
bool(true)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
float(INF)
bool(true)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(true)
