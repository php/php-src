--TEST--
Oracle Database 12c Implicit Result Sets: oci_get_implicit_resultset: oci_cancel
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

$plsql =
    "declare
      c1 sys_refcursor;
      c2 sys_refcursor;
    begin
      open c1 for select 1 from dual union all select 2 from dual;
      dbms_sql.return_result(c1);
      open c2 for select 3 from dual;
      dbms_sql.return_result (c2);
    end;";

// Run Test

echo "Test 1\n";
$s = oci_parse($c, $plsql);
oci_execute($s);
while (($s1 = oci_get_implicit_resultset($s))) {
    while (($row = oci_fetch_array($s1, OCI_ASSOC+OCI_RETURN_NULLS)) != false) {
        foreach ($row as $item) {
            echo "  ".$item;
        }
        echo "\n";
        oci_cancel($s1);
    }
}

?>
===DONE===
<?php exit(0); ?>
--EXPECT--
Test 1
  1
  3
===DONE===
