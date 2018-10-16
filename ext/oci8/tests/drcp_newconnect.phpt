--TEST--
DRCP: oci_new_connect()
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--INI--
oci8.connection_class=test
oci8.old_oci_close_semantics=0
--FILE--
<?php

require dirname(__FILE__)."/details.inc";

// Open two connections with oci_new_connect
// Verify they are different by comparing the resource ids

var_dump($c1 = oci_new_connect($user,$password,$dbase));
$rn1 = (int)$c1;

// Another connection now

var_dump($c2 = oci_new_connect($user,$password,$dbase));
$rn2 = (int)$c2;

// rn1 and rn2 should be different.

if ($rn1 === $rn2)
	echo "First and second connections share a resource: Not OK\n";
else
	echo "First and second connections are different  OK\n";

// Close the connections
oci_close($c1);
oci_close($c2);

echo "Done\n";

?>
--EXPECTF--
resource(%d) of type (oci8 connection)
resource(%d) of type (oci8 connection)
First and second connections are different  OK
Done
