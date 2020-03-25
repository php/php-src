--TEST--
Oracle Database 12c Implicit Result Sets: oci_get_implicit_resultset: get from wrong statement
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

function print_row($row)
{
    foreach ($row as $item) {
        echo "  ".$item;
    }
    echo "\n";
}

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
// This test effectively discards all the first IRS results
$s = oci_parse($c, $plsql);
oci_execute($s);
while (($s1 = oci_get_implicit_resultset($s))) {  // $s1 is never used again so its results are lost
    while (($row = oci_fetch_array($s, OCI_ASSOC+OCI_RETURN_NULLS)) != false) {  // use parent $s instead of $s1
        print_row($row);
    }
}
oci_free_statement($s);

echo "\nTest 2 - fetch first IRS explicitly\n";
$s = oci_parse($c, $plsql);
oci_execute($s);
$s1 = oci_get_implicit_resultset($s);
while (($row = oci_fetch_row($s1)) != false) {
    print_row($row);
}
while (($row = oci_fetch_row($s)) != false) {
    print_row($row);
}
oci_free_statement($s);

echo "\nTest 3 - fetch part of IRS explicitly\n";
$s = oci_parse($c, $plsql);
oci_execute($s);
$s1 = oci_get_implicit_resultset($s);
while (($row = oci_fetch_row($s1)) != false) {
    print_row($row);
}
$row = oci_fetch_row($s);
print_row($row);
$s1 = oci_get_implicit_resultset($s);
while (($row = oci_fetch_row($s1)) != false) {
    print_row($row);
}
while (($row = oci_fetch_row($s)) != false) {
    print_row($row);
}
oci_free_statement($s);

echo "\nTest 4 - skip IRSs\n";
$s = oci_parse($c, $plsql);
oci_execute($s);
$s1 = oci_get_implicit_resultset($s);
$s1 = oci_get_implicit_resultset($s);
while (($row = oci_fetch_row($s)) != false) { // parent
    print_row($row);
}
oci_free_statement($s);

?>
--EXPECT--
Test 1
  3
  4
  5
  6

Test 2 - fetch first IRS explicitly
  1
  2
  3
  4
  5
  6

Test 3 - fetch part of IRS explicitly
  1
  2
  3
  5
  6
  4

Test 4 - skip IRSs
  5
  6
