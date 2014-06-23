--TEST--
Bug #35014 (array_product() always returns 0) (64bit)
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
Warning: array_product() expects parameter 1 to be array, string given in /Users/ajf/Projects/2014/PHP/php-src/ext/standard/tests/array/bug35014_64bit.php on line 15
NULL
int(1)
int(0)
int(3)
int(9)
float(1)
int(9999999800000001)
float(1.219953680145E+30)
int(36893488147419103230)