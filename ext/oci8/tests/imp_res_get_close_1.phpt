--TEST--
Oracle Database 12c Implicit Result Sets: oci_get_implicit_resultset: oci_free_statement #1
--SKIPIF--
<?php
if (!extension_loaded('oci8')) die ("skip no oci8 extension");
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(__DIR__.'/skipif.inc');
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

require(__DIR__.'/connect.inc');

// Initialization

$plsql =
    "declare
      c1 sys_refcursor;
    begin
      open c1 for select 1 from dual union all select 2 from dual;
      dbms_sql.return_result(c1);
      open c1 for select 3 from dual union all select 4 from dual;
      dbms_sql.return_result(c1);
      open c1 for select 5 from dual union all select 6 from dual;
      dbms_sql.return_result(c1);
    end;";

// Run Test

echo "Test 1\n";

$s = oci_parse($c, $plsql);
oci_execute($s);

try {
    while (($s1 = oci_get_implicit_resultset($s))) {
        while (($row = oci_fetch_array($s1, OCI_ASSOC+OCI_RETURN_NULLS)) != false) {
            foreach ($row as $item) {
                echo "  ".$item;
            }
            echo "\n";
            oci_free_statement($s1);  // Free the implicit result handle
        }
    }
    oci_free_statement($s);
} catch(\TypeError $exception) {
    var_dump($exception->getMessage());
}

?>
--EXPECTF--
Test 1
  1
string(%d) "oci_fetch_array(): supplied resource is not a valid oci8 statement resource"