--TEST--
Bug #36392 (wrong number of decimal digits with %e specifier in sprintf)
--FILE--
<?php
	echo sprintf("%e\n", 1.123456789);
	echo sprintf("%.10e\n", 1.123456789);
	echo sprintf("%.0e\n", 1.123456789);
	echo sprintf("%.1e\n", 1.123456789);
	echo sprintf("%5.1e\n", 1.123456789);
?>
--EXPECT--
1.123457e+0
1.1234567890e+0
1e+0
1.1e+0
1.1e+0
