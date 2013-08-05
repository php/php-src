--TEST--
ocipasswordchange()
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on thes
require(dirname(__FILE__).'/connect.inc');
if (empty($dbase)) die ("skip requires database connection string be set");
if ($test_drcp) die("skip password change not supported in DRCP Mode");

preg_match('/.*Release ([[:digit:]]+)\.([[:digit:]]+)\.([[:digit:]]+)\.([[:digit:]]+)\.([[:digit:]]+)*/', oci_server_version($c), $matches_sv);
if (isset($matches_sv[1]) && $matches_sv[1] >= 11) {
    preg_match('/([[:digit:]]+)\.([[:digit:]]+)\.([[:digit:]]+)\.([[:digit:]]+)\.([[:digit:]]+)/', oci_client_version(), $matches);
    if (isset($matches[0]) && $matches[1] == 10 && $matches[2] == 2 && $matches[3] == 0 && $matches[4] < 5) { 
        die ("skip test known to fail using Oracle 10.2.0.4 client libs connecting to Oracle 11 (6277160)");
    }
}

// This test in Oracle 12c needs a non-CDB or the root container
if (isset($matches_sv[1]) && $matches_sv[1] >= 12) {
    $s = oci_parse($c, "select nvl(sys_context('userenv', 'con_name'), 'notacdb') as dbtype from dual");
    $r = @oci_execute($s);
    if (!$r)
        die('skip could not identify container type');
    $r = oci_fetch_array($s);
    if ($r['DBTYPE'] !== 'CDB$ROOT')
        die('skip cannot run test using a PDB');
}
?>
--FILE--
<?php

// This test will diff if either the client or the server is 11.2.0.3
// (or greater) and the other is 11.2.0.2 (or earlier).  Both client
// and server must be upgraded at the same time.

require dirname(__FILE__)."/connect.inc";

$new_password = "test";
var_dump(ocipasswordchange($dbase, $user, $password, $new_password));

if (!empty($dbase)) {
	var_dump($new_c = ocilogon($user,$new_password,$dbase));
}
else {
	var_dump($new_c = ocilogon($user,$new_password));
}

var_dump(ocipasswordchange($dbase, $user, $new_password, $password));


echo "Done\n";

?>
--EXPECTF--
resource(%d) of type (oci8 connection)
resource(%d) of type (oci8 connection)
resource(%d) of type (oci8 connection)
Done
