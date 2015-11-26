--TEST--
closing temporary lobs
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(dirname(__FILE__).'/skipif.inc');
?> 
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
