--TEST--
ocipasswordchange()
--SKIPIF--
<?php
if (!extension_loaded('oci8')) die("skip no oci8 extension"); 
require dirname(__FILE__)."/connect.inc";
if (empty($dbase)) die ("skip requires database connection string be set");

// This test is known to fail with Oracle 10g client libraries
// connecting to Oracle Database 11.1.0.6 (Oracle bug 6277160)
$sv = oci_server_version($c);
$sv = preg_match('/11.1/', $sv, $matches);
if ($sv === 1) {
	ob_start();
	phpinfo(INFO_MODULES);
	$phpinfo = ob_get_clean();
	$iv = preg_match('/Oracle .*Version => 10/', $phpinfo);
	if ($iv === 1) {
		die ("skip test known to fail using Oracle 10gR2 client libs connecting to Oracle 11.1 (6277160)");
	}
}
?>
--FILE--
<?php

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
