--TEST--
ocilogon()
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php

require dirname(__FILE__)."/connect.inc";

if (!empty($dbase)) {
	var_dump(ocilogon($user, $password, $dbase));
}
else {
	var_dump(ocilogon($user, $password));
}
	
echo "Done\n";

?>
--EXPECTF--
resource(%d) of type (oci8 connection)
Done
