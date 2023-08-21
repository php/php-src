--TEST--
DRCP: oci_pconnect() with scope end when oci8.old_oci_close_semantics ON
--EXTENSIONS--
oci8
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--INI--
oci8.old_oci_close_semantics=1
--FILE--
<?php

require __DIR__."/drcp_functions.inc";
require __DIR__."/details.inc";

// The test opens a connection within function1 and updates a table
// (without committing).  Another connection is opened from function
// 2, and the table queried.  When function1 ends, the connection from
// function1 is not closed, so the updated value will be seen in
// function2.  Also the table can't be dropped because an uncommitted
// transaction exists.

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
Deprecated: Directive oci8.old_oci_close_semantics is deprecated%s
This is with a OCI_PCONNECT
resource(%d) of type (oci8 persistent connection)
Update done-- DEPT value has been set to NEWDEPT
resource(%d) of type (oci8 persistent connection)
The value of DEPT for id 105 is NEWDEPT

Warning: oci_execute(): ORA-00054: %s
Done
