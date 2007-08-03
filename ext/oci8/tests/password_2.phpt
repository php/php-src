--TEST--
oci_password_change() for persistent connections
--SKIPIF--
<?php 
if (!extension_loaded('oci8')) die("skip no oci8 extension"); 
require(dirname(__FILE__)."/details.inc");
if (strcasecmp($user, "system") && strcasecmp($user, "sys")) die("skip needs to be run as a DBA user"); 
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

// Connect (persistent) and change the password
$c1 = oci_pconnect("testuser", "testuserpwd", $dbase);
var_dump($c1);

ob_start();
var_dump($c1);
$r1 = ob_get_clean();
preg_match("/resource\(([0-9])\) of.*/", $r1, $matches);
$rn1 = $matches[1]; /* resource number */

oci_password_change($c1, "testuser", "testuserpwd", "testuserpwd2");

// Second connect should return a new resource because the hash string will be different from $c1
$c2 = oci_pconnect("testuser", "testuserpwd2", $dbase);
var_dump($c2);

ob_start();
var_dump($c2);
$r2 = ob_get_clean();
preg_match("/resource\(([0-9])\) of.*/", $r2, $matches);
$rn2 = $matches[1]; /* resource number */

// Despite using the old password this connect should succeed and return the original resource
$c3 = oci_pconnect("testuser", "testuserpwd", $dbase);  
var_dump($c3);

ob_start();
var_dump($c3);
$r3 = ob_get_clean();
preg_match("/resource\(([0-9])\) of.*/", $r3, $matches);
$rn3 = $matches[1]; /* resource number */

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
// Can't drop a user that is connected and can't close a persistent
// connection.  So this test will leave the dummy user around, but the
// schema will not be usable..
$s = oci_parse($c0, "revoke connect, create session from testuser");
@oci_execute($s);

echo "Done\n";

?>
--EXPECTF--
resource(%d) of type (oci8 persistent connection)
resource(%d) of type (oci8 persistent connection)
resource(%d) of type (oci8 persistent connection)
First and second connections are different: OK
First and third connections share a resource: OK
Done
