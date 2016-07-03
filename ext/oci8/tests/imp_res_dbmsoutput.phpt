--TEST--
Oracle Database 12c Implicit Result Sets: interleaved with DBMS_OUTPUT
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
    "drop table imp_res_dbmsoutput_tab_1",
    "create table imp_res_dbmsoutput_tab_1 (c1 number, c2 varchar2(10))",
    "insert into imp_res_dbmsoutput_tab_1 values (1, 'abcde')",
    "insert into imp_res_dbmsoutput_tab_1 values (2, 'fghij')",
    "insert into imp_res_dbmsoutput_tab_1 values (3, 'klmno')",

    "drop table imp_res_dbmsoutput_tab_2",
    "create table imp_res_dbmsoutput_tab_2 (c3 varchar2(1))",
    "insert into imp_res_dbmsoutput_tab_2 values ('t')",
    "insert into imp_res_dbmsoutput_tab_2 values ('u')",
    "insert into imp_res_dbmsoutput_tab_2 values ('v')",

    "create or replace procedure imp_res_dbmsoutput_proc as
      c1 sys_refcursor;
    begin
      dbms_output.put_line('dbms_output Line 1');
      open c1 for select * from imp_res_dbmsoutput_tab_1 order by 1;
      dbms_sql.return_result(c1);
      dbms_output.put_line('dbms_output Line 2');
      open c1 for select * from imp_res_dbmsoutput_tab_2 order by 1;
      dbms_sql.return_result(c1);
    end;"
);

oci8_test_sql_execute($c, $stmtarray);

function setserveroutputon($c)
{
    $s = oci_parse($c, "begin dbms_output.enable(null); end;");
    oci_execute($s);
}

function getdbmsoutput_do($c)
{
    $s = oci_parse($c, "begin dbms_output.get_line(:ln, :st); end;");
    oci_bind_by_name($s, ":ln", $ln, 100);
    oci_bind_by_name($s, ":st", $st, -1, SQLT_INT);
    $res = false;
    while (($succ = oci_execute($s)) && !$st) {
        $res[] = $ln;  // append each line to the array
    }
    return $res;
}

setserveroutputon($c);

// Run Test

echo "Test 1\n";
$s = oci_parse($c, "begin imp_res_dbmsoutput_proc(); end;");
oci_execute($s);
var_dump(getdbmsoutput_do($c));
while (($row = oci_fetch_array($s, OCI_ASSOC+OCI_RETURN_NULLS)) != false) {
    foreach ($row as $item) {
        echo "  ".$item;
    }
    echo "\n";
}

echo "\nTest 2\n";
$s = oci_parse($c, "begin imp_res_dbmsoutput_proc(); end;");
oci_execute($s);
while (($row = oci_fetch_array($s, OCI_ASSOC+OCI_RETURN_NULLS)) != false) {
    foreach ($row as $item) {
        echo "  ".$item;
    }
    echo "\n";
}
var_dump(getdbmsoutput_do($c));

// Clean up

$stmtarray = array(
    "drop procedure imp_res_dbmsoutput_proc",
    "drop table imp_res_dbmsoutput_tab_1",
    "drop table imp_res_dbmsoutput_tab_2"
);

oci8_test_sql_execute($c, $stmtarray);

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
Test 1
array(2) {
  [0]=>
  string(18) "dbms_output Line 1"
  [1]=>
  string(18) "dbms_output Line 2"
}
  1  abcde
  2  fghij
  3  klmno
  t
  u
  v

Test 2
  1  abcde
  2  fghij
  3  klmno
  t
  u
  v
array(2) {
  [0]=>
  string(18) "dbms_output Line 1"
  [1]=>
  string(18) "dbms_output Line 2"
}
===DONE===
