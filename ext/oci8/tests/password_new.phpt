--TEST--
oci_password_change()
--EXTENSIONS--
oci8
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(__DIR__.'/skipif.inc');
require(__DIR__.'/connect.inc');
if (empty($dbase)) die ("skip requires database connection string be set");
if ($test_drcp) die("skip password change not supported in DRCP Mode");

preg_match('/.*Release ([[:digit:]]+)\.([[:digit:]]+)\.([[:digit:]]+)\.([[:digit:]]+)\.([[:digit:]]+)*/', oci_server_version($c), $matches_sv);
preg_match('/([[:digit:]]+)\.([[:digit:]]+)\.([[:digit:]]+)\.([[:digit:]]+)\.([[:digit:]]+)/', oci_client_version(), $matches);
if (!(isset($matches_sv[0]) && isset($matches[0])
       && $matches_sv[1] == $matches[1]
       && $matches_sv[2] == $matches[2]
       && $matches_sv[3] == $matches[3]
       && $matches_sv[4] == $matches[4])) {
           // Avoid diffs due to cross version protocol changes (e.g. like 11.2.0.2-11.2.0.3) and bugs like Oracle bug: 6277160
           die ("skip test only runs when database client libraries and database server are the same version");
}

// This test in Oracle 12c needs a non-CDB or the root container
if (isset($matches_sv[0]) && $matches_sv[1] >= 12) {
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

require __DIR__."/connect.inc";

$new_password = "test";
var_dump(oci_password_change($dbase, $user, $password, $new_password));
var_dump($new_c = oci_connect($user,$new_password,$dbase));
var_dump(oci_password_change($dbase, $user, $new_password, $password));


echo "Done\n";

?>
--EXPECTF--
resource(%d) of type (oci8 connection)
resource(%d) of type (oci8 connection)
resource(%d) of type (oci8 connection)
Done
