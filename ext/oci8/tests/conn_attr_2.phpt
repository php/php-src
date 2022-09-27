--TEST--
Set and get of connection attributes across persistent connections and sysdba connection.
--EXTENSIONS--
oci8
--SKIPIF--
<?php
if (getenv('SKIP_REPEAT')) die('skip fails with repeat');
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(__DIR__.'/skipif.inc');

if (strcasecmp($user, "system") && strcasecmp($user, "sys")) die("skip needs to be run as a DBA user");
if ($test_drcp) die("skip output might vary with DRCP");

preg_match('/.*Release ([[:digit:]]+)\.([[:digit:]]+)\.([[:digit:]]+)\.([[:digit:]]+)\.([[:digit:]]+)*/', oci_server_version($c), $matches);
if (!(isset($matches[0]) && $matches[1] >= 10)) {
    die("skip expected output only valid when using Oracle 10g or greater database server");
}
?>
--INI--
oci8.privileged_connect = On
--FILE--
<?php

$testuser     = 'testuser_attr_2';  // Used in conn_attr.inc
$testpassword = 'testuser';

require(__DIR__."/conn_attr.inc");

$attr_array = array('MODULE','ACTION','CLIENT_INFO','CLIENT_IDENTIFIER');

echo"**Set values using pconnect-1**\n";

var_dump($pc1 = oci_pconnect($testuser,$testpassword,$dbase));
foreach($attr_array as $attr) {
    set_attr($pc1,$attr,100);
}

//  using pc1 again
echo"\n**Get values using pconnect-2**\n";
var_dump($pc3 = oci_pconnect($testuser,$testpassword,$dbase));
foreach($attr_array as $attr) {
    get_attr($pc3,$attr);
}

// Get with different pconnect
echo"\n**Get values using pconnect-3**\n";
var_dump($pc2 = oci_pconnect($testuser,$testpassword,$dbase,'UTF8'));
foreach($attr_array as $attr) {
    get_attr($pc2,$attr);
}

oci_close($pc1);
oci_close($pc2);
oci_close($pc3);

// Re-open a persistent connection and check for the attr values.
echo "\n**Re-open a pconnect()**\n";
var_dump($pc4 = oci_pconnect($testuser,$testpassword,$dbase));
foreach($attr_array as $attr) {
    get_attr($pc4,$attr);
}
oci_close($pc4);

// Test with SYSDBA connection.
echo "\n**Test with SYSDBA connection**\n";
$sys_c1 = @oci_pconnect($testuser,$testpassword,$dbase,false,OCI_SYSDBA);
var_dump($sys_c1);
if (!$sys_c1) {
    $e = oci_error();
    if ($e['code'] != 1031 && $e['code'] != 1017) {
        var_dump($e);
    }
} else {
    set_attr($sys_c1,'ACTION',10);
    get_sys_attr($sys_c1,'ACTION');
    get_attr($sys_c1,'ACTION');
    oci_close($sys_c1);
}

clean_up($c);

echo "Done\n";
?>
--EXPECTF--
**Set values using pconnect-1**
resource(%d) of type (oci8 persistent connection)
Value of MODULE has been set successfully
Value of ACTION has been set successfully
Value of CLIENT_INFO has been set successfully
Value of CLIENT_IDENTIFIER has been set successfully

**Get values using pconnect-2**
resource(%d) of type (oci8 persistent connection)
The value of MODULE is PHP TEST100
The value of ACTION is TASK100
The value of CLIENT_INFO is INFO1100
The value of CLIENT_IDENTIFIER is ID00100

**Get values using pconnect-3**
resource(%d) of type (oci8 persistent connection)
The value of MODULE is %s
The value of ACTION is 
The value of CLIENT_INFO is 
The value of CLIENT_IDENTIFIER is 

**Re-open a pconnect()**
resource(%d) of type (oci8 persistent connection)
The value of MODULE is PHP TEST100
The value of ACTION is TASK100
The value of CLIENT_INFO is INFO1100
The value of CLIENT_IDENTIFIER is ID00100

**Test with SYSDBA connection**
bool(false)
Done
