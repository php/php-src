--TEST--
ocipasswordchange()
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on thes
require(dirname(__FILE__).'/skipif.inc');
if (empty($dbase)) die ("skip requires database connection string be set");
if ($test_drcp) die("skip password change not supported in DRCP Mode");

// This test is known to fail with Oracle 10.2.0.4 client libraries
// connecting to Oracle Database 11 (Oracle bug 6277160, fixed 10.2.0.5)
if (preg_match('/Release (11|12)\./', oci_server_version($c), $matches) === 1 &&
    preg_match('/^10\.2\.0\.[1234]/', oci_client_version()) === 1) {
    die ("skip test known to fail using Oracle 10.2.0.4 client libs connecting to Oracle 11 (6277160)");
}


?>
--FILE--
<?php

// This test will diff if either the client or the server is 11.2.0.3
// (or greater) and the other is 11.2.0.2 (or earlier).  Both client
// and server must be upgraded at the same time.

require dirname(__FILE__)."/connect.inc";

$new_password = "test";
var_dump(ocipasswordchange($dbase, $user, $password, $new_password));

if (!empty($dbase)) {
	var_dump($new_c = ocilogon($user,$new_password,$dbase));
}
else {
	var_dump($new_c = ocilogon($user,$new_password));
}

var_dump(ocipasswordchange($dbase, $user, $new_password, $password));


echo "Done\n";

?>
--EXPECTF--
resource(%d) of type (oci8 connection)
resource(%d) of type (oci8 connection)
resource(%d) of type (oci8 connection)
Done
