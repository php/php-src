--TEST--
Bug #24640 (var_export and var_dump can't output large float)
--FILE--
<?php
function test($v)
{
	echo var_export($v, true) . "\n";
	var_dump($v);
	echo "$v\n";
	print_r($v);
	echo "\n------\n";
}

test(1.7e+300);
test(1.7e-300);
test(1.7e+79);
test(1.7e-79);
test(1.7e+80);
test(1.7e-80);
test(1.7e+81);
test(1.7e-81);
?>
--EXPECT--
1.7E+300
float(1.7E+300)
1.7E+300
1.7E+300
------
1.7E-300
float(1.7E-300)
1.7E-300
1.7E-300
------
1.7E+79
float(1.7E+79)
1.7E+79
1.7E+79
------
1.7E-79
float(1.7E-79)
1.7E-79
1.7E-79
------
1.7E+80
float(1.7E+80)
1.7E+80
1.7E+80
------
1.7E-80
float(1.7E-80)
1.7E-80
1.7E-80
------
1.7E+81
float(1.7E+81)
1.7E+81
1.7E+81
------
1.7E-81
float(1.7E-81)
1.7E-81
1.7E-81
------
