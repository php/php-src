--TEST--
unserialize() floats with E notation (#18654)
--POST--
--GET--
--FILE--
<?php 
foreach(array(1e2, 5.2e25, 85.29e-23, 9e-9) AS $value) {
	echo ($ser = serialize($value))."\n";
	var_dump(unserialize($ser));
	echo "\n";
}
?>
--EXPECT--
d:100;
float(100)

d:5.2E+25;
float(5.2E+25)

d:8.528999999999999914734871708787977695465087890625E-22;
float(8.529E-22)

d:9.E-9;
float(9.E-9)
