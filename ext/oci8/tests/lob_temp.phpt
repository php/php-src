--TEST--
temporary lobs
--EXTENSIONS--
oci8
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require __DIR__.'/skipif.inc';
?>
--FILE--
<?php

require __DIR__.'/connect.inc';

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
--EXPECT--
bool(true)
string(4) "test"
bool(true)
string(2) "te"
bool(true)
string(4) "test"
bool(true)
string(2) "te"
Done
