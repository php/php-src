--TEST--
closing temporary lobs
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--ENV--
return "
ORACLE_HOME=".(isset($_ENV['ORACLE_HOME']) ? $_ENV['ORACLE_HOME'] : '')."
NLS_LANG=".(isset($_ENV['NLS_LANG']) ? $_ENV['NLS_LANG'] : '')."
";
--FILE--
<?php
	
require dirname(__FILE__).'/connect.inc';

$blob = oci_new_descriptor($c,OCI_D_LOB);
var_dump($blob->writeTemporary("test"));
var_dump($blob->load());
var_dump($blob->close());

$c = oci_pconnect($user, $password, $dbase);

$blob = oci_new_descriptor($c,OCI_D_LOB);
var_dump($blob->writeTemporary("test"));
var_dump($blob->load());
var_dump($blob->close());

echo "Done\n";

?>
--EXPECTF--
bool(true)
string(4) "test"
bool(true)
bool(true)
string(4) "test"
bool(true)
Done
