--TEST--
PECL Bug #6109 (Error messages not kept)
--SKIPIF--
<?php if (!extension_loaded('oci8')) die ("skip no oci8 extension"); ?>
--FILE--
<?php

require(dirname(__FILE__).'/connect.inc');

// Run Test

echo "Test 1\n";

$s = oci_parse($c, 'delete from table_does_not_exist');
$r = @oci_execute($s);

if ($r) {
	echo "whoops - table does exist\n";
} else {
	for ($i = 0; $i < 5; $i++) {
		$err = oci_error($s);
		echo ($i) .' -> '.$err['message'] ."\n";
	}
}

// Cleanup

oci_close($c);

echo "Done\n";

?>
--EXPECTF--
Test 1
0 -> ORA-00942: %s
1 -> ORA-00942: %s
2 -> ORA-00942: %s
3 -> ORA-00942: %s
4 -> ORA-00942: %s
Done
