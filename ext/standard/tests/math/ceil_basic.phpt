--TEST--
Test ceil() - basic function test for ceil()
--INI--
precision=14
--SKIPIF--
if (strtolower(PHP_OS) == 'darwin') {
    die('SKIP OSX does weird things with -0 so this test doesn't work there');
}
--FILE--
<?php
/* Prototype  : float ceil  ( float $value  )
 * Description: Round fractions up.
 * Source code: ext/standard/math.c
 */

echo "*** Testing ceil() : basic functionality ***\n";
$values = array(0,
				-0,
				0.5,
				-0.5,
				1,
				-1,
				1.5,
				-1.5,
				2.6,
				-2.6,
				037,
				0x5F,	
				"10.5",
				"-10.5",
				"3.95E3",
				"-3.95E3",
				"039",
				"0x5F",
				true,
				false,
				null, 
				);	

for ($i = 0; $i < count($values); $i++) {
	$res = ceil($values[$i]);
	var_dump($res);
}

?>
===Done===
--EXPECTF--
*** Testing ceil() : basic functionality ***
float(0)
float(0)
float(1)
float(-0)
float(1)
float(-1)
float(2)
float(-1)
float(3)
float(-2)
float(31)
float(95)
float(11)
float(-10)
float(3950)
float(-3950)
float(39)
float(95)
float(1)
float(0)
float(0)
===Done===
