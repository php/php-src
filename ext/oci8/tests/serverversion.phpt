--TEST--
oci_server_version()
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php

require dirname(__FILE__)."/connect.inc";

if (!empty($dbase)) {
	var_dump($c = oci_connect($user, $password, $dbase));
}
else {
	var_dump($c = oci_connect($user, $password));
}

$v = oci_server_version($c);
var_dump(str_replace("\n", "", $v));

$v = ociserverversion($c);
var_dump(str_replace("\n", "", $v));

echo "Done\n";

?>
--EXPECTF--
resource(%d) of type (oci8 connection)
string(%d) "Oracle %s"
string(%d) "Oracle %s"
Done
