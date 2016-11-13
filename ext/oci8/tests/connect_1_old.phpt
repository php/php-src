--TEST--
ociplogon() & ocinlogon()
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php

require __DIR__."/connect.inc";

if (!empty($dbase)) {
	var_dump($c1 = ociplogon($user, $password, $dbase));
}
else {
	var_dump($c1 = ociplogon($user, $password));
}

if (!empty($dbase)) {
	var_dump($c2 = ocinlogon($user, $password, $dbase));
}
else {
	var_dump($c2 = ocinlogon($user, $password));
}

var_dump(ocilogoff($c1));
var_dump(ocilogoff($c2));

echo "Done\n";

?>
--EXPECTF--
resource(%d) of type (oci8 persistent connection)
resource(%d) of type (oci8 connection)
bool(true)
bool(true)
Done
