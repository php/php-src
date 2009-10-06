--TEST--
PECL Bug #6109 (Error messages not kept)
--SKIPIF--
<?php if (!extension_loaded('oci8')) die ("skip no oci8 extension"); ?>
--FILE--
<?php

require(dirname(__FILE__).'/connect.inc');

// Run Test

echo "Test 1\n";

echo "Once Oracle has popped the message from its stack, the message is\n";
echo "no longer available.\n";


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
Once Oracle has popped the message from its stack, the message is
no longer available.
0 -> ORA-00942: %s
1 -> 
2 -> 
3 -> 
4 -> 
Done
