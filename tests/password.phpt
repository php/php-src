--TEST--
oci_password_change() for non-persistent connections
--SKIPIF--
<?php 
if (!extension_loaded('oci8')) die("skip no oci8 extension"); 
require(dirname(__FILE__)."/details.inc");
if (empty($dbase)) die ("skip requires database connection string be set");
if (strcasecmp($user, "system") && strcasecmp($user, "sys")) die("skip needs to be run as a DBA user"); 
if ($test_drcp) die("skip password change not supported in DRCP Mode");
?>
--FILE--
<?php

require(dirname(__FILE__)."/details.inc");

// Create a user we can stuff around with and not affect subsequent tests
$c0 = oci_connect($user, $password, $dbase);
$stmts = array(
	"drop user testuser",
	"begin
	 execute immediate 'create user testuser identified by testuserpwd';
	 execute immediate 'grant connect, create session to testuser';
	end;");
foreach ($stmts as $sql) {
	$s = oci_parse($c0, $sql);
	@oci_execute($s);
}

// Connect and change the password
$c1 = oci_connect("testuser", "testuserpwd", $dbase);
var_dump($c1);
$rn1 = (int)$c1;

oci_password_change($c1, "testuser", "testuserpwd", "testuserpwd2");

// Second connect should return a new resource because the hash string will be different from $c1
$c2 = oci_connect("testuser", "testuserpwd2", $dbase);
var_dump($c2);
$rn2 = (int)$c2;

// Despite using the old password this connect should succeed and return the original resource
$c3 = oci_connect("testuser", "testuserpwd", $dbase);  
var_dump($c3);
$rn3 = (int)$c3;

// Connections should differ
if ($rn1 == $rn2) {
	echo "First and second connections share a resource: Not OK\n";
	var_dump($c1);
}
else {
	echo "First and second connections are different: OK\n";
}

// Connections should be the same
if ($rn1 == $rn3) {
	echo "First and third connections share a resource: OK\n";
}
else {
	echo "First and third connections are different: Not OK\n";
	var_dump($c1);
	var_dump($c2);
}

// Clean up
oci_close($c1);
oci_close($c2);
oci_close($c3);

// Clean up
$s = oci_parse($c0, "drop user cascade testuser");
@oci_execute($s);

echo "Done\n";

?>
--EXPECTF--
resource(%d) of type (oci8 connection)
resource(%d) of type (oci8 connection)
resource(%d) of type (oci8 connection)
First and second connections are different: OK
First and third connections share a resource: OK
Done
