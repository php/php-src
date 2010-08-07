--TEST--
DRCP: oci_pconnect() with scope end when oci8.old_oci_close_semantics OFF
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--INI--
oci8.old_oci_close_semantics=0
--FILE--
<?php

require dirname(__FILE__)."/drcp_functions.inc";
require dirname(__FILE__)."/details.inc";

// The default expected behavior of this test is different between PHP
// 5.2 and PHP 5.3
//
// In PHP 5.3, the test opens a connection within function1 and
// updates a table (without committing).  Another connection is opened
// from function 2, and the table queried.  When function1 ends, the
// txn is rolled back and hence the updated value will not be
// reflected in function2.  Use oci8.old_oci_close_semantics=1 to
// get old behavior

// Create the table
$c = oci_new_connect($user,$password,$dbase);
@drcp_drop_table($c);
drcp_create_table($c);

echo "This is with a OCI_PCONNECT\n";
function1($user,$password,$dbase);

// Should return the OLD value
function2($user,$password,$dbase);

// This is the first scope for the script

function function1($user,$password,$dbase)
{
	var_dump($c = oci_pconnect($user,$password,$dbase));
	drcp_update_table($c);
}

// This is the second scope

function function2($user,$password,$dbase)
{
	var_dump($c = oci_pconnect($user,$password,$dbase));
	drcp_select_value($c);
}

drcp_drop_table($c);
oci_close($c);

echo "Done\n";

?>
--EXPECTF--
This is with a OCI_PCONNECT
resource(%d) of type (oci8 persistent connection)
Update done-- DEPT value has been set to NEWDEPT
resource(%d) of type (oci8 persistent connection)
The value of DEPT for id 105 is HR
Done
