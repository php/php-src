--TEST--
Oracle Database 12c Implicit Result Sets: oci_get_implicit_resultset: interleaved fetches
--SKIPIF--
<?php 
if (!extension_loaded('oci8')) die ("skip no oci8 extension"); 
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
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

// Initialization

$stmtarray = array(
    "drop table imp_res_get_4_tab_1",
    "create table imp_res_get_4_tab_1 (c1 number, c2 varchar2(10))",
    "insert into imp_res_get_4_tab_1 values (1, 'abcde')",
    "insert into imp_res_get_4_tab_1 values (2, 'fghij')",
    "insert into imp_res_get_4_tab_1 values (3, 'klmno')",

    "drop table imp_res_get_4_tab_2",
    "create table imp_res_get_4_tab_2 (c3 varchar2(1))",
    "insert into imp_res_get_4_tab_2 values ('t')",
    "insert into imp_res_get_4_tab_2 values ('u')",
    "insert into imp_res_get_4_tab_2 values ('v')",

    "create or replace procedure imp_res_get_4_proc as
      c1 sys_refcursor;
    begin
      open c1 for select * from imp_res_get_4_tab_1 order by 1;
      dbms_sql.return_result(c1);

      open c1 for select * from imp_res_get_4_tab_2 order by 1;
      dbms_sql.return_result(c1);
    end;"
);

oci8_test_sql_execute($c, $stmtarray);

function print_row($row)
{
    if ($row === false) {
      print "Return is false\n";
      return;
    }
    foreach ($row as $item) {
        echo "  ".$item;
    }
    echo "\n";
}

// Run Test

echo "Test 1\n";

$s = oci_parse($c, "begin imp_res_get_4_proc(); end;");
oci_execute($s);
$s1 = oci_get_implicit_resultset($s);
$s2 = oci_get_implicit_resultset($s);
$row = oci_fetch_array($s1, OCI_ASSOC+OCI_RETURN_NULLS);
print_row($row);
$row = oci_fetch_array($s2, OCI_ASSOC+OCI_RETURN_NULLS);
print_row($row);
$row = oci_fetch_array($s1, OCI_ASSOC+OCI_RETURN_NULLS);
print_row($row);
$row = oci_fetch_array($s2, OCI_ASSOC+OCI_RETURN_NULLS);
print_row($row);
$row = oci_fetch_array($s1, OCI_ASSOC+OCI_RETURN_NULLS);
print_row($row);
$row = oci_fetch_array($s2, OCI_ASSOC+OCI_RETURN_NULLS);
print_row($row);

echo "Test 2 - too many fetches\n";

$s = oci_parse($c, "begin imp_res_get_4_proc(); end;");
oci_execute($s);
$s1 = oci_get_implicit_resultset($s);
$s2 = oci_get_implicit_resultset($s);
$row = oci_fetch_array($s1, OCI_ASSOC+OCI_RETURN_NULLS);
print_row($row);
$row = oci_fetch_array($s2, OCI_ASSOC+OCI_RETURN_NULLS);
print_row($row);
$row = oci_fetch_array($s1, OCI_ASSOC+OCI_RETURN_NULLS);
print_row($row);
$row = oci_fetch_array($s2, OCI_ASSOC+OCI_RETURN_NULLS);
print_row($row);
$row = oci_fetch_array($s1, OCI_ASSOC+OCI_RETURN_NULLS);
print_row($row);
$row = oci_fetch_array($s2, OCI_ASSOC+OCI_RETURN_NULLS);
print_row($row);
$row = oci_fetch_array($s1, OCI_ASSOC+OCI_RETURN_NULLS);
print_row($row);
$row = oci_fetch_array($s2, OCI_ASSOC+OCI_RETURN_NULLS);
print_row($row);
$row = oci_fetch_array($s1, OCI_ASSOC+OCI_RETURN_NULLS);
print_row($row);
$row = oci_fetch_array($s2, OCI_ASSOC+OCI_RETURN_NULLS);
print_row($row);

// Clean up

$stmtarray = array(
    "drop procedure imp_res_get_4_proc",
    "drop table imp_res_get_4_tab_1",
    "drop table imp_res_get_4_tab_2"
);

oci8_test_sql_execute($c, $stmtarray);

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
Test 1
  1  abcde
  t
  2  fghij
  u
  3  klmno
  v
Test 2 - too many fetches
  1  abcde
  t
  2  fghij
  u
  3  klmno
  v
Return is false
Return is false

Warning: oci_fetch_array(): ORA-01002: %s in %simp_res_get_4.php on line %d
Return is false

Warning: oci_fetch_array(): ORA-01002: %s in %simp_res_get_4.php on line %d
Return is false
===DONE===
