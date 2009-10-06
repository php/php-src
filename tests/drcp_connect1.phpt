--TEST--
DRCP: oci_connect()
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
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

var_dump($conn1 = oci_connect($user,$password,$dbase));
// Create the package
drcp_create_package($conn1);
     
// OCI_CONNECT
echo " This is with OCI_CONNECT.....\n";
drcp_select_packagevar($conn1); // Returns 0
drcp_set_packagevar($conn1,1000);
oci_close($conn1);
echo " Connection conn1  closed....\n";

// Second connection should return 0 for the package variable.
var_dump($conn2 = oci_connect($user,$password,$dbase));
echo " Select with connection 2 \n";
drcp_select_packagevar($conn2); // Returns 0
drcp_set_packagevar($conn2,100);

// Third connection. There is no oci_close() for conn2 hence this should
// return the value set by conn2.
var_dump($conn3 = oci_connect($user,$password,$dbase));
echo " Select with connection 3 \n";
drcp_select_packagevar($conn3); // Returns 100

// Close all the connections
oci_close($conn2);
oci_close($conn3);

// OCI_PCONNECT
echo "\n This is with oci_pconnect().....\n";
var_dump($pconn1 = oci_pconnect($user,$password,$dbase));
drcp_set_packagevar($pconn1,1000);
oci_close($pconn1);
echo " Connection pconn1  closed....\n";

// Second connection with oci_pconnect should return the same session hence the
// value returned is what is set by pconn1

var_dump($pconn2 = oci_pconnect($user,$password,$dbase));
echo " Select with persistent connection 2 \n";
drcp_select_packagevar($pconn2); // Returns 1000
oci_close($pconn2);

echo "Done\n";

?>
--EXPECTF--
resource(%d) of type (oci8 connection)
 This is with OCI_CONNECT.....
 The value of the package variable is 0
 Package variable value set to 1000
 Connection conn1  closed....
resource(%d) of type (oci8 connection)
 Select with connection 2 
 The value of the package variable is 0
 Package variable value set to 100
resource(%d) of type (oci8 connection)
 Select with connection 3 
 The value of the package variable is 100

 This is with oci_pconnect().....
resource(%d) of type (oci8 persistent connection)
 Package variable value set to 1000
 Connection pconn1  closed....
resource(%d) of type (oci8 persistent connection)
 Select with persistent connection 2 
 The value of the package variable is 1000
Done

