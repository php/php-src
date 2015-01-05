--TEST--
Bug #35014 (array_product() always returns 0) (64bit)
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only"); ?>
--INI--
precision=14
--FILE--
<?php
$tests = array(
	'foo',
	array(),
	array(0),
	array(3),
	array(3, 3),
	array(0.5, 2),
	array(99999999, 99999999),
	array(8.993, 7443241,988, sprintf("%u", -1)+0.44),
	array(2,sprintf("%u", -1)),
);

foreach ($tests as $v) {
	var_dump(array_product($v));
}
?>
--EXPECTF--	
Warning: array_product() expects parameter 1 to be array, string given in %s on line %d
NULL
int(1)
int(0)
int(3)
int(9)
float(1)
int(9999999800000001)
float(1.219953680145E+30)
float(3.6893488147419E+19)
