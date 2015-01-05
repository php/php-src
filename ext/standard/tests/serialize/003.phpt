--TEST--
unserialize() floats with E notation (#18654)
--INI--
precision=12
serialize_precision=100
--FILE--
<?php 
foreach(array(1e2, 5.2e25, 85.29e-23, 9e-9) AS $value) {
	echo ($ser = serialize($value))."\n";
	var_dump(unserialize($ser));
	echo "\n";
}
?>
--EXPECTREGEX--
d:100;
float\(100\)

d:5[0-9]*;
float\(5\.2E\+25\)

d:8\.52[89][0-9]+E-22;
float\(8\.529E-22\)

d:8\.[9]*[0-9]*E-9;
float\(9\.0E-9\)
