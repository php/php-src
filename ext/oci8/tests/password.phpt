--TEST--
oci_password_change() for non-persistent connections
--EXTENSIONS--
oci8
--SKIPIF--
<?php
require(__DIR__."/details.inc");
if (empty($dbase)) die ("skip requires database connection string be set");
if (strcasecmp($user, "system") && strcasecmp($user, "sys")) die("skip needs to be run as a DBA user");
if ($test_drcp) die("skip password change not supported in DRCP Mode");
?>
--FILE--
<?php

require(__DIR__."/connect.inc");

$stmtarray = array(
    "drop user testuser_pw cascade",
    "create user testuser_pw identified by testuserpwd",
    "grant connect, create session to testuser_pw"
);

oci8_test_sql_execute($c, $stmtarray);


// Connect and change the password
$c1 = oci_connect("testuser_pw", "testuserpwd", $dbase);
var_dump($c1);
$rn1 = (int)$c1;

oci_password_change($c1, "testuser_pw", "testuserpwd", "testuserpwd2");

// Second connect should return a new resource because the hash string will be different from $c1
$c2 = oci_connect("testuser_pw", "testuserpwd2", $dbase);
var_dump($c2);
$rn2 = (int)$c2;

// Despite using the old password this connect should succeed and return the original resource
$c3 = oci_connect("testuser_pw", "testuserpwd", $dbase);
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

echo "Done\n";

?>
--CLEAN--
<?php

require(__DIR__."/connect.inc");

$stmtarray = array(
    "drop user testuser_pw cascade"
);

oci8_test_sql_execute($c, $stmtarray);

?>
--EXPECTF--
resource(%d) of type (oci8 connection)
resource(%d) of type (oci8 connection)
resource(%d) of type (oci8 connection)
First and second connections are different: OK
First and third connections share a resource: OK
Done
