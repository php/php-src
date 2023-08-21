--TEST--
DRCP: oci_new_connect() and oci_connect() with scope end when oci8.old_oci_close_semantics ON
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

// Scope considered here is the  functional scope
// Test will open a connection within a function (function 1).
// Update a table
// Open another connection from function 2.
// When the scope ends the txn is rolled back and hence the updated value
// will not be  reflected for oci_connect and oci_new_connect.

// Create the table
$c = oci_new_connect($user,$password,$dbase);
@drcp_drop_table($c);
drcp_create_table($c);

// OCI_NEW_CONNECT
$conn_type = 1;
echo "This is with a OCI_NEW_CONNECT\n";
function1($user,$password,$dbase,$conn_type);

// Should return the OLD value
function2($user,$password,$dbase,$conn_type);

// OCI_CONNECT
$conn_type = 2;
echo "\n\nThis is with a OCI_CONNECT\n";
function1($user,$password,$dbase,$conn_type);

// Should return the OLD value
function2($user,$password,$dbase,$conn_type);

//This is the first scope for the script

function function1($user,$password,$dbase,$conn_type)
{
    switch($conn_type)
    {
    case 1:
        var_dump($conn1 = oci_new_connect($user,$password,$dbase));
        break;
    case 2:
        var_dump($conn1 = oci_connect($user,$password,$dbase));
        break;
    }
    drcp_update_table($conn1);
}

// This is the second scope

function function2($user,$password,$dbase,$conn_type)
{
    switch($conn_type)
    {
    case 1:
        var_dump($conn1 = oci_new_connect($user,$password,$dbase));
        break;
    case 2:
        var_dump($conn1 = oci_connect($user,$password,$dbase));
        break;
    }
    drcp_select_value($conn1);
}

drcp_drop_table($c);
oci_close($c);

echo "Done\n";

?>
--EXPECTF--
Deprecated: Directive oci8.old_oci_close_semantics is deprecated%s
This is with a OCI_NEW_CONNECT
resource(%d) of type (oci8 connection)
Update done-- DEPT value has been set to NEWDEPT
resource(%d) of type (oci8 connection)
The value of DEPT for id 105 is HR


This is with a OCI_CONNECT
resource(%d) of type (oci8 connection)
Update done-- DEPT value has been set to NEWDEPT
resource(%d) of type (oci8 connection)
The value of DEPT for id 105 is HR
Done
