--TEST--
Set and get of connection attributes with oci_close().
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(dirname(__FILE__).'/skipif.inc');

if (strcasecmp($user, "system") && strcasecmp($user, "sys")) die("skip needs to be run as a DBA user");
if ($test_drcp) die("skip output might vary with DRCP");

preg_match('/.*Release ([[:digit:]]+)\.([[:digit:]]+)\.([[:digit:]]+)\.([[:digit:]]+)\.([[:digit:]]+)*/', oci_server_version($c), $matches);
if (!(isset($matches[0]) && $matches[1] >= 10)) {
	die("skip expected output only valid when using Oracle 10g or greater database server");
}
?>
--FILE--
<?php

$testuser     = 'testuser_attr_3';  // Used in conn_attr.inc
$testpassword = 'testuser';

require(dirname(__FILE__)."/conn_attr.inc");

echo"**Test Set and get values for  the attributes with oci_close() ************\n";
// With oci_connect ,oci_pconnect ,oci_new_connect

var_dump($conn1 = get_conn(1)); //oci_connect()
set_attr($conn1,'ACTION',1);
get_attr($conn1,'ACTION');
oci_close($conn1);

// Open another connect and verify the value.
var_dump($conn1 = get_conn(1)); //oci_connect()
get_attr($conn1,'ACTION');
oci_close($conn1);

var_dump($pconn1 = get_conn(2)); //oci_pconnect()
set_attr($pconn1,'MODULE',2);
get_attr($pconn1,'MODULE');
oci_close($pconn1);

// Open another connect and verify the value.
var_dump($pconn1 = get_conn(2)); //oci_pconnect()
get_attr($pconn1,'MODULE');
oci_close($pconn1);

var_dump($nconn1 = get_conn(3)); //oci_new_connect()
set_attr($nconn1,'CLIENT_INFO',3);
set_attr($nconn1,'CLIENT_IDENTIFIER',3);
get_attr($nconn1,'CLIENT_INFO');
get_attr($nconn1,'CLIENT_IDENTIFIER');
oci_close($nconn1);

// Open another connect and verify the value.
var_dump($nconn1 = get_conn(3)); //oci_new_connect()
get_attr($nconn1,'CLIENT_INFO');
get_attr($nconn1,'CLIENT_IDENTIFIER');
oci_close($nconn1);

clean_up($c);
echo "Done\n";

?>
--EXPECTF--
**Test Set and get values for  the attributes with oci_close() ************
Testing with oci_connect()
resource(%d) of type (oci8 connection)
Value of ACTION has been set successfully
The value of ACTION is TASK1
Testing with oci_connect()
resource(%d) of type (oci8 connection)
The value of ACTION is 
Testing with oci_pconnect()
resource(%d) of type (oci8 persistent connection)
Value of MODULE has been set successfully
The value of MODULE is PHP TEST2
Testing with oci_pconnect()
resource(%d) of type (oci8 persistent connection)
The value of MODULE is PHP TEST2
Testing with oci_new_connect()
resource(%d) of type (oci8 connection)
Value of CLIENT_INFO has been set successfully
Value of CLIENT_IDENTIFIER has been set successfully
The value of CLIENT_INFO is INFO13
The value of CLIENT_IDENTIFIER is ID003
Testing with oci_new_connect()
resource(%d) of type (oci8 connection)
The value of CLIENT_INFO is 
The value of CLIENT_IDENTIFIER is 
Done
