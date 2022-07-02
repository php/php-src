--TEST--
DRCP: oci_connect() with oci_close() and oci8.old_oci_close_semantics OFF
--EXTENSIONS--
oci8
--INI--
oci8.old_oci_close_semantics=0
oci8.connection_class=test
--FILE--
<?php

require __DIR__."/details.inc";

// Test will open a connection
// Close the connection
// Open another connection
// With oci_close() the connection is released to the pool and hence the
// the second connection will be different


// OCI_CONNECT
echo "This is with a OCI_CONNECT\n";
var_dump($conn1 = oci_connect($user,$password,$dbase));
$rn1 = (int)$conn1;
oci_close($conn1);

// Open another connection
var_dump($conn2 = oci_connect($user,$password,$dbase));
$rn2 = (int)$conn2;
oci_close($conn2);

// Compare the resource numbers

if ($rn1 === $rn2)
    echo "Both connections share a resource : NOT OK\n";
else
    echo "Both connections are different : OK\n";

echo "Done\n";

?>
--EXPECTF--
This is with a OCI_CONNECT
resource(%d) of type (oci8 connection)
resource(%d) of type (oci8 connection)
Both connections are different : OK
Done
