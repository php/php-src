--TEST--
zend_dval_to_lval preserves low bits  (32 bit long)
--SKIPIF--
<?php
if (PHP_INT_SIZE != 4)
	 die("skip for machines with 32-bit longs");
?>
--FILE--
<?php
	/* test doubles around -4e21 */
	$values = [
		-4000000000000001048576.,
		-4000000000000000524288.,
		-4000000000000000000000.,
		-3999999999999999475712.,
		-3999999999999998951424.,
	];
	/* see if we're rounding negative numbers right */
	$values[] = -2147483649.8;

	foreach ($values as $v) {
		var_dump((int)$v);
	}

?>
--EXPECT--
int(-2056257536)
int(-2055733248)
int(-2055208960)
int(-2054684672)
int(-2054160384)
int(2147483647)
