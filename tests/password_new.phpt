--TEST--
oci_password_change()
--SKIPIF--
<?php
if (!extension_loaded('oci8')) die("skip no oci8 extension"); 
require(dirname(__FILE__)."/connect.inc");
if (empty($dbase)) die ("skip requires database connection string be set");
if ($test_drcp) die("skip password change not supported in DRCP Mode");

// This test is known to fail with Oracle 10.2.0.4 client libraries
// connecting to Oracle Database 11 (Oracle bug 6277160, fixed 10.2.0.5)
$sv = oci_server_version($c);
$sv = preg_match('/Release (11|12)\./', $sv, $matches);
if ($sv === 1) {
	ob_start();
	phpinfo(INFO_MODULES);
	$phpinfo = ob_get_clean();
	$iv = preg_match('/Oracle .*Version => 10/', $phpinfo);
	if ($iv === 1) {
		die ("skip test known to fail using Oracle 10.2.0.4 client libs connecting to Oracle 11 (6277160)");
	}
}
?>
--FILE--
<?php

require dirname(__FILE__)."/connect.inc";

$new_password = "test";
var_dump(oci_password_change($dbase, $user, $password, $new_password));

if (!empty($dbase)) {
	var_dump($new_c = ocilogon($user,$new_password,$dbase));
}
else {
	var_dump($new_c = ocilogon($user,$new_password));
}

var_dump(oci_password_change($dbase, $user, $new_password, $password));


echo "Done\n";

?>
--EXPECTF--
resource(%d) of type (oci8 connection)
resource(%d) of type (oci8 connection)
resource(%d) of type (oci8 connection)
Done
