--TEST--
Test pow() - basic function test pow()
--INI--
precision=14
--SKIPIF--
<?php
if (PHP_INT_SIZE != 4) die("skip this test is for 32bit platform only");
?>
--FILE--
<?php
$bases = array(23,
				-23,
				23.1,
				-23.1,
				2.345e1,
				-2.345e1,
				0x17,
				027,
				"23",
				"23.45",
				"2.345e1",	
				PHP_INT_MAX,
				-PHP_INT_MAX - 1);			

$exponents = array(0,
               1,
               -1,
               2,
               -2,
               3,
               -3,
               2.5,
               -2.5,
               500,
               -500,
               2147483647,
			   -2147483648); 				
					
foreach($bases as $base) {
	echo "\n\nBase = $base";
	foreach($exponents as $exponent) {
		echo "\n..... Exponent = $exponent Result = ";
		$res = pow($base, $exponent);
		echo $res;
	}
	echo "\n\n";
}
?>
===Done===
--EXPECT--
Base = 23
..... Exponent = 0 Result = 1
..... Exponent = 1 Result = 23
..... Exponent = -1 Result = 0.043478260869565
..... Exponent = 2 Result = 529
..... Exponent = -2 Result = 0.001890359168242
..... Exponent = 3 Result = 12167
..... Exponent = -3 Result = 8.2189529053999E-5
..... Exponent = 2.5 Result = 2536.9948758324
..... Exponent = -2.5 Result = 0.00039416713432339
..... Exponent = 500 Result = INF
..... Exponent = -500 Result = 0
..... Exponent = 2147483647 Result = INF
..... Exponent = -2147483648 Result = 0



Base = -23
..... Exponent = 0 Result = 1
..... Exponent = 1 Result = -23
..... Exponent = -1 Result = -0.043478260869565
..... Exponent = 2 Result = 529
..... Exponent = -2 Result = 0.001890359168242
..... Exponent = 3 Result = -12167
..... Exponent = -3 Result = -8.2189529053999E-5
..... Exponent = 2.5 Result = NAN
..... Exponent = -2.5 Result = NAN
..... Exponent = 500 Result = INF
..... Exponent = -500 Result = 0
..... Exponent = 2147483647 Result = -INF
..... Exponent = -2147483648 Result = 0



Base = 23.1
..... Exponent = 0 Result = 1
..... Exponent = 1 Result = 23.1
..... Exponent = -1 Result = 0.043290043290043
..... Exponent = 2 Result = 533.61
..... Exponent = -2 Result = 0.0018740278480538
..... Exponent = 3 Result = 12326.391
..... Exponent = -3 Result = 8.1126746668997E-5
..... Exponent = 2.5 Result = 2564.6608940579
..... Exponent = -2.5 Result = 0.00038991509650141
..... Exponent = 500 Result = INF
..... Exponent = -500 Result = 0
..... Exponent = 2147483647 Result = INF
..... Exponent = -2147483648 Result = 0



Base = -23.1
..... Exponent = 0 Result = 1
..... Exponent = 1 Result = -23.1
..... Exponent = -1 Result = -0.043290043290043
..... Exponent = 2 Result = 533.61
..... Exponent = -2 Result = 0.0018740278480538
..... Exponent = 3 Result = -12326.391
..... Exponent = -3 Result = -8.1126746668997E-5
..... Exponent = 2.5 Result = NAN
..... Exponent = -2.5 Result = NAN
..... Exponent = 500 Result = INF
..... Exponent = -500 Result = 0
..... Exponent = 2147483647 Result = -INF
..... Exponent = -2147483648 Result = 0



Base = 23.45
..... Exponent = 0 Result = 1
..... Exponent = 1 Result = 23.45
..... Exponent = -1 Result = 0.042643923240938
..... Exponent = 2 Result = 549.9025
..... Exponent = -2 Result = 0.001818504189379
..... Exponent = 3 Result = 12895.213625
..... Exponent = -3 Result = 7.7548153065204E-5
..... Exponent = 2.5 Result = 2662.9138571162
..... Exponent = -2.5 Result = 0.00037552848257846
..... Exponent = 500 Result = INF
..... Exponent = -500 Result = 0
..... Exponent = 2147483647 Result = INF
..... Exponent = -2147483648 Result = 0



Base = -23.45
..... Exponent = 0 Result = 1
..... Exponent = 1 Result = -23.45
..... Exponent = -1 Result = -0.042643923240938
..... Exponent = 2 Result = 549.9025
..... Exponent = -2 Result = 0.001818504189379
..... Exponent = 3 Result = -12895.213625
..... Exponent = -3 Result = -7.7548153065204E-5
..... Exponent = 2.5 Result = NAN
..... Exponent = -2.5 Result = NAN
..... Exponent = 500 Result = INF
..... Exponent = -500 Result = 0
..... Exponent = 2147483647 Result = -INF
..... Exponent = -2147483648 Result = 0



Base = 23
..... Exponent = 0 Result = 1
..... Exponent = 1 Result = 23
..... Exponent = -1 Result = 0.043478260869565
..... Exponent = 2 Result = 529
..... Exponent = -2 Result = 0.001890359168242
..... Exponent = 3 Result = 12167
..... Exponent = -3 Result = 8.2189529053999E-5
..... Exponent = 2.5 Result = 2536.9948758324
..... Exponent = -2.5 Result = 0.00039416713432339
..... Exponent = 500 Result = INF
..... Exponent = -500 Result = 0
..... Exponent = 2147483647 Result = INF
..... Exponent = -2147483648 Result = 0



Base = 23
..... Exponent = 0 Result = 1
..... Exponent = 1 Result = 23
..... Exponent = -1 Result = 0.043478260869565
..... Exponent = 2 Result = 529
..... Exponent = -2 Result = 0.001890359168242
..... Exponent = 3 Result = 12167
..... Exponent = -3 Result = 8.2189529053999E-5
..... Exponent = 2.5 Result = 2536.9948758324
..... Exponent = -2.5 Result = 0.00039416713432339
..... Exponent = 500 Result = INF
..... Exponent = -500 Result = 0
..... Exponent = 2147483647 Result = INF
..... Exponent = -2147483648 Result = 0



Base = 23
..... Exponent = 0 Result = 1
..... Exponent = 1 Result = 23
..... Exponent = -1 Result = 0.043478260869565
..... Exponent = 2 Result = 529
..... Exponent = -2 Result = 0.001890359168242
..... Exponent = 3 Result = 12167
..... Exponent = -3 Result = 8.2189529053999E-5
..... Exponent = 2.5 Result = 2536.9948758324
..... Exponent = -2.5 Result = 0.00039416713432339
..... Exponent = 500 Result = INF
..... Exponent = -500 Result = 0
..... Exponent = 2147483647 Result = INF
..... Exponent = -2147483648 Result = 0



Base = 23.45
..... Exponent = 0 Result = 1
..... Exponent = 1 Result = 23.45
..... Exponent = -1 Result = 0.042643923240938
..... Exponent = 2 Result = 549.9025
..... Exponent = -2 Result = 0.001818504189379
..... Exponent = 3 Result = 12895.213625
..... Exponent = -3 Result = 7.7548153065204E-5
..... Exponent = 2.5 Result = 2662.9138571162
..... Exponent = -2.5 Result = 0.00037552848257846
..... Exponent = 500 Result = INF
..... Exponent = -500 Result = 0
..... Exponent = 2147483647 Result = INF
..... Exponent = -2147483648 Result = 0



Base = 2.345e1
..... Exponent = 0 Result = 1
..... Exponent = 1 Result = 23.45
..... Exponent = -1 Result = 0.042643923240938
..... Exponent = 2 Result = 549.9025
..... Exponent = -2 Result = 0.001818504189379
..... Exponent = 3 Result = 12895.213625
..... Exponent = -3 Result = 7.7548153065204E-5
..... Exponent = 2.5 Result = 2662.9138571162
..... Exponent = -2.5 Result = 0.00037552848257846
..... Exponent = 500 Result = INF
..... Exponent = -500 Result = 0
..... Exponent = 2147483647 Result = INF
..... Exponent = -2147483648 Result = 0



Base = 2147483647
..... Exponent = 0 Result = 1
..... Exponent = 1 Result = 2147483647
..... Exponent = -1 Result = 4.6566128752458E-10
..... Exponent = 2 Result = 4.6116860141324E+18
..... Exponent = -2 Result = 2.1684043469905E-19
..... Exponent = 3 Result = 9.903520300448E+27
..... Exponent = -3 Result = 1.0097419600935E-28
..... Exponent = 2.5 Result = 2.1370991100146E+23
..... Exponent = -2.5 Result = 4.6792401686657E-24
..... Exponent = 500 Result = INF
..... Exponent = -500 Result = 0
..... Exponent = 2147483647 Result = INF
..... Exponent = -2147483648 Result = 0



Base = -2147483648
..... Exponent = 0 Result = 1
..... Exponent = 1 Result = -2147483648
..... Exponent = -1 Result = -4.6566128730774E-10
..... Exponent = 2 Result = 4.6116860184274E+18
..... Exponent = -2 Result = 2.168404344971E-19
..... Exponent = 3 Result = -9.903520314283E+27
..... Exponent = -3 Result = -1.0097419586829E-28
..... Exponent = 2.5 Result = NAN
..... Exponent = -2.5 Result = NAN
..... Exponent = 500 Result = INF
..... Exponent = -500 Result = 0
..... Exponent = 2147483647 Result = -INF
..... Exponent = -2147483648 Result = 0

===Done===