--TEST--
temporary lobs
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php
	
require dirname(__FILE__).'/connect.inc';

$blob = oci_new_descriptor($c,OCI_D_LOB);
var_dump($blob->writeTemporary("test"));
var_dump($blob->load());
var_dump($blob->seek(0, SEEK_SET));
var_dump($blob->read(2));

$c = oci_pconnect($user, $password, $dbase);

$blob = oci_new_descriptor($c,OCI_D_LOB);
var_dump($blob->writeTemporary("test"));
var_dump($blob->load());
var_dump($blob->seek(0, SEEK_SET));
var_dump($blob->read(2));

echo "Done\n";

?>
--EXPECTF--
bool(true)
string(4) "test"
bool(true)
string(2) "te"
bool(true)
string(4) "test"
bool(true)
string(2) "te"
Done
