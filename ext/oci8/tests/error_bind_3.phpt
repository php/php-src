--TEST--
Test some more oci_bind_by_name error conditions
--SKIPIF--
<?php
if (!extension_loaded('oci8')) die ("skip no oci8 extension");
$target_dbs = array('oracledb' => true, 'timesten' => true);  // test runs on these DBs
require(dirname(__FILE__).'/skipif.inc');
preg_match('/.*Release ([[:digit:]]+)\.([[:digit:]]+)\.([[:digit:]]+)\.([[:digit:]]+)\.([[:digit:]]+)*/', oci_server_version($c), $matches);
if (!(isset($matches[0]) && $matches[1] >= 12)) {
    die("skip expected output only valid when using Oracle Database 12c or greater");
}
preg_match('/^[[:digit:]]+/', oci_client_version(), $matches);
if (!(isset($matches[0]) && $matches[0] >= 12)) {
    die("skip works only with Oracle 12c or greater version of Oracle client libraries");
}
?>
--FILE--
<?php

require(dirname(__FILE__).'/connect.inc');

$sql = "begin
        :output1 := 99;
        :output2 := 'abc';
       end;";

$s = oci_parse($c, $sql);
oci_bind_by_name($s, ':output1', $output1, -1, OCI_B_BOL);
oci_bind_by_name($s, ':output2', $output2, -1, OCI_B_BOL);
oci_execute($s);
var_dump($output1);
var_dump($output2);

echo "Done\n";

?>
--EXPECTF--
Warning: oci_execute(): ORA-06550: line %d, column %d:
PLS-00382: %s
ORA-06550: line %d, column %d:
PL/SQL: %s
ORA-06550: line %d, column %d:
PLS-00382: %s
ORA-06550: line %d, column %d:
PL/SQL: %s in %s on line %d
bool(false)
bool(false)
Done
