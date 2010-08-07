--TEST--
DRCP: oci_pconnect() with oci_close() and oci8.old_oci_close_semantics OFF
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--INI--
oci8.old_oci_close_semantics=0
oci8.connection_class=test
--FILE--
<?php

require dirname(__FILE__)."/details.inc";

// Test will open a persistent connection
// Close the connection
// Open another connection
// With oci_close() the connection is released to the pool and hence the
// the second connection will be different


echo "This is with a OCI_PCONNECT\n";
var_dump($conn1 = oci_pconnect($user,$password,$dbase));
$rn1 = (int)$conn1;
oci_close($conn1);

// Query for the row updated. The new value should be returned

var_dump($conn2 = oci_pconnect($user,$password,$dbase));
$rn2 = (int)$conn2;
oci_close($conn2);

// Compare the resource numbers

if ($rn1 === $rn2)
	echo "Both connections share a resource : NOT OK \n";
else
	echo "Both connections are different : OK \n";

echo "Done\n";

?>
--EXPECTF--
This is with a OCI_PCONNECT
resource(%d) of type (oci8 persistent connection)
resource(%d) of type (oci8 persistent connection)
Both connections are different : OK 
Done
