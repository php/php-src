--TEST--
float EXPECTF specifiers
--FILE--
<?php
$vs = [
	PHP_INT_MAX * 2,
	PHP_INT_MIN * 2,
	(string) PHP_INT_MAX * 2,
	(string) PHP_INT_MIN * 2,
	NAN,
];

foreach ($vs as $v) {
	echo $v, "\n";
}
?>
--EXPECTF--
%f
%f
%f
%f
