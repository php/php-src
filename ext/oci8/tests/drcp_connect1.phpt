--TEST--
DRCP: oci_connect()
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs (Calling PL/SQL from SQL is not supported in TimesTen)
require(dirname(__FILE__).'/skipif.inc');
?>
--INI--
oci8.connection_class=test
oci8.old_oci_close_semantics=0
--FILE--
<?php

require dirname(__FILE__)."/details.inc";
require dirname(__FILE__)."/drcp_functions.inc";

// Open a number of connections with oci_connect and oci_pconnect and verify
// whether we get a used session with DRCP.
// To verify this, we change the value of a PL/SQL package variable in one
// session and query for this through another connection

echo "Test 1a\n";
var_dump($conn1 = oci_connect($user,$password,$dbase));
// Create the package
drcp_create_package($conn1);

echo "Test 1b\n";
// OCI_CONNECT
echo " This is with OCI_CONNECT.....\n";
drcp_select_packagevar($conn1); // Returns 0
drcp_set_packagevar($conn1,1000);
oci_close($conn1);
echo " Connection conn1  closed....\n";

echo "Test 2\n";
// Second connection should return 0 for the package variable.
var_dump($conn2 = oci_connect($user,$password,$dbase));
echo " Select with connection 2\n";
drcp_select_packagevar($conn2); // Returns 0
drcp_set_packagevar($conn2,100);

echo "Test 3\n";
// Third connection. There is no oci_close() for conn2 hence this should
// return the value set by conn2.
var_dump($conn3 = oci_connect($user,$password,$dbase));
echo " Select with connection 3\n";
drcp_select_packagevar($conn3); // Returns 100

// Close all the connections
oci_close($conn2);
oci_close($conn3);

echo "Test 4\n";
// OCI_PCONNECT
echo " This is with oci_pconnect().....\n";
var_dump($pconn1 = oci_pconnect($user,$password,$dbase));
drcp_set_packagevar($pconn1,1000);
oci_close($pconn1);
echo " Connection pconn1  closed....\n";

// Second connection with oci_pconnect should return the same session hence the
// value returned is what is set by pconn1

echo "Test 5\n";
var_dump($pconn2 = oci_pconnect($user,$password,$dbase));
echo " Select with persistent connection 2\n";
drcp_select_packagevar($pconn2); // Returns 1000
oci_close($pconn2);

echo "Done\n";

?>
--EXPECTF--
Test 1a
resource(%d) of type (oci8 connection)
Test 1b
 This is with OCI_CONNECT.....
 The value of the package variable is 0
 Package variable value set to 1000
 Connection conn1  closed....
Test 2
resource(%d) of type (oci8 connection)
 Select with connection 2
 The value of the package variable is 0
 Package variable value set to 100
Test 3
resource(%d) of type (oci8 connection)
 Select with connection 3
 The value of the package variable is 100
Test 4
 This is with oci_pconnect().....
resource(%d) of type (oci8 persistent connection)
 Package variable value set to 1000
 Connection pconn1  closed....
Test 5
resource(%d) of type (oci8 persistent connection)
 Select with persistent connection 2
 The value of the package variable is 1000
Done
