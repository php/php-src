--TEST--
Verify that the Driver Name attribute is set
--EXTENSIONS--
oci8
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
require __DIR__.'/connect.inc';
if (strcasecmp($user, "system") && strcasecmp($user, "sys")) die("skip needs to be run as a DBA user");
if ($test_drcp) die("skip as Output might vary with DRCP");

preg_match('/.*Release ([[:digit:]]+)\.([[:digit:]]+)\.([[:digit:]]+)\.([[:digit:]]+)\.([[:digit:]]+)*/', oci_server_version($c), $matches);
if (!(isset($matches[0]) && ($matches[1] == 11 && $matches[2] >= 2))) {
    die("skip expected output only valid when using Oracle 11.2 database or its later patchsets");
}
preg_match('/^([[:digit:]]+)\.([[:digit:]]+)\.([[:digit:]]+)\.([[:digit:]]+)\.([[:digit:]]+)/', oci_client_version(), $matches);
if (!(isset($matches[0]) && ($matches[1] == 11 && $matches[2] >= 2) || ($matches[1] > 11))) {
    die("skip test expected to work only with Oracle 11.2 or greater version of client");
}

?>
--FILE--
<?php
require __DIR__.'/connect.inc';

echo"**Test 1.1 - Default values for the attribute **************\n";
get_attr($c);
oci_close($c);

echo"\n***Test 1.2 - Get the values from different connections **************\n";
// with oci_pconnect()
echo "Testing with oci_pconnect()\n";
$pc1=oci_pconnect($user,$password,$dbase);
get_attr($pc1);
oci_close($pc1);

echo "Testing with oci_new_connect()\n";
$nc1=oci_new_connect($user,$password,$dbase);
get_attr($nc1);
oci_close($nc1);
echo "Done\n";

function get_attr($conn)
{
    $sel_stmt = "select client_driver
        from v\$session_connect_info sci, v\$session s
        where sci.client_driver is not null
          and sci.sid = s.sid
          and s.audsid = userenv('SESSIONID')";
    $s2 = oci_parse($conn,$sel_stmt);
    oci_execute($s2,OCI_DEFAULT);
    oci_fetch($s2);
    echo "The value of DRIVER_NAME is ".trim(oci_result($s2,1))."\n";
}

?>
--EXPECT--
**Test 1.1 - Default values for the attribute **************
The value of DRIVER_NAME is PHP OCI8

***Test 1.2 - Get the values from different connections **************
Testing with oci_pconnect()
The value of DRIVER_NAME is PHP OCI8
Testing with oci_new_connect()
The value of DRIVER_NAME is PHP OCI8
Done
