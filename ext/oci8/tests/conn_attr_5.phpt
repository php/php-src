--TEST--
Set and get connection attributes with scope end.
--EXTENSIONS--
oci8
--SKIPIF--
<?php
if (getenv('SKIP_REPEAT')) die('skip fails with repeat');
require_once 'skipifconnectfailure.inc';
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require __DIR__.'/skipif.inc';

if (strcasecmp($user, "system") && strcasecmp($user, "sys")) die("skip needs to be run as a DBA user");
if ($test_drcp) die("skip output might vary with DRCP");

preg_match('/.*Release ([[:digit:]]+)\.([[:digit:]]+)\.([[:digit:]]+)\.([[:digit:]]+)\.([[:digit:]]+)*/', oci_server_version($c), $matches);
if (!(isset($matches[0]) && $matches[1] >= 10)) {
    die("skip expected output only valid when using Oracle 10g or greater database server");
}
?>
--FILE--
<?php

$testuser     = 'testuser_attr_5';  // Used in conn_attr.inc
$testpassword = 'testuser';

require __DIR__."/conn_attr.inc";

echo"**Test - Set and get values for the attributes with scope end ************\n";

// Set the attributes in one scope and verify the values from another scope.
set_scope();

echo "Get the Values from a different scope \n";
get_scope();

function set_scope() {
    $conn1 = get_conn(1);
    set_attr($conn1,'CLIENT_INFO',50);
    set_attr($conn1,'CLIENT_IDENTIFIER',50);
    $conn2 = get_conn(3);
    set_attr($conn2,'ACTION',50);
    $conn3 = get_conn(2);
    set_attr($conn3,'MODULE',50);
}

function get_scope() {
    $conn1 = get_conn(1);
    get_attr($conn1,'CLIENT_INFO');
    get_attr($conn1,'CLIENT_IDENTIFIER');
    $conn2 = get_conn(3);
    get_attr($conn2,'ACTION');
    $conn3 = get_conn(2);
    get_attr($conn3,'MODULE');
}
clean_up($c);
echo "Done";
?>
--EXPECT--
**Test - Set and get values for the attributes with scope end ************
Testing with oci_connect()
Value of CLIENT_INFO has been set successfully
Value of CLIENT_IDENTIFIER has been set successfully
Testing with oci_new_connect()
Value of ACTION has been set successfully
Testing with oci_pconnect()
Value of MODULE has been set successfully
Get the Values from a different scope 
Testing with oci_connect()
The value of CLIENT_INFO is 
The value of CLIENT_IDENTIFIER is 
Testing with oci_new_connect()
The value of ACTION is 
Testing with oci_pconnect()
The value of MODULE is PHP TEST50
Done
