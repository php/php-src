--TEST--
Oracle Database 12c Implicit Result Sets: oci_get_implicit_resultset: basic test 3
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
--INI--
oci8.statement_cache_size = 0
--FILE--
<?php

require(dirname(__FILE__).'/connect.inc');

// Initialization

$stmtarray = array(
    "drop table imp_res_get_3_tab_1",
    "create table imp_res_get_3_tab_1 (c1 number, c2 varchar2(10))",
    "insert into imp_res_get_3_tab_1 values (1, 'abcde')",
    "insert into imp_res_get_3_tab_1 values (2, 'fghij')",
    "insert into imp_res_get_3_tab_1 values (3, 'klmno')",

    "drop table imp_res_get_3_tab_2",
    "create table imp_res_get_3_tab_2 (c3 varchar2(1))",
    "insert into imp_res_get_3_tab_2 values ('t')",
    "insert into imp_res_get_3_tab_2 values ('u')",
    "insert into imp_res_get_3_tab_2 values ('v')",

    "create or replace procedure imp_res_get_3_proc as
      c1 sys_refcursor;
      i pls_integer;
    begin
      for i in 1..30 loop  -- if this value is too big for Oracle's open_cursors, calling imp_res_get_3_proc() can fail with ORA-1000
        open c1 for select * from imp_res_get_3_tab_1 order by 1;
        dbms_sql.return_result(c1);
        open c1 for select * from imp_res_get_3_tab_2 where rownum < 3 order by 1;
        dbms_sql.return_result(c1);
        open c1 for select * from dual;
        dbms_sql.return_result (c1);
      end loop;
    end;"
);

oci8_test_sql_execute($c, $stmtarray);

// Run Test

echo "Test 1\n";

$s = oci_parse($c, "begin imp_res_get_3_proc(); end;");
oci_execute($s);

while (($s1 = oci_get_implicit_resultset($s))) {
    while (($row = oci_fetch_array($s1, OCI_ASSOC+OCI_RETURN_NULLS)) != false) {
        foreach ($row as $item) {
            echo "  ".$item;
        }
        echo "\n";
    }
}

// Clean up

$stmtarray = array(
    "drop procedure imp_res_get_3_proc",
    "drop table imp_res_get_3_tab_1",
    "drop table imp_res_get_3_tab_2"
);

oci8_test_sql_execute($c, $stmtarray);

?>
===DONE===
<?php exit(0); ?>
--EXPECT--
Test 1
  1  abcde
  2  fghij
  3  klmno
  t
  u
  X
  1  abcde
  2  fghij
  3  klmno
  t
  u
  X
  1  abcde
  2  fghij
  3  klmno
  t
  u
  X
  1  abcde
  2  fghij
  3  klmno
  t
  u
  X
  1  abcde
  2  fghij
  3  klmno
  t
  u
  X
  1  abcde
  2  fghij
  3  klmno
  t
  u
  X
  1  abcde
  2  fghij
  3  klmno
  t
  u
  X
  1  abcde
  2  fghij
  3  klmno
  t
  u
  X
  1  abcde
  2  fghij
  3  klmno
  t
  u
  X
  1  abcde
  2  fghij
  3  klmno
  t
  u
  X
  1  abcde
  2  fghij
  3  klmno
  t
  u
  X
  1  abcde
  2  fghij
  3  klmno
  t
  u
  X
  1  abcde
  2  fghij
  3  klmno
  t
  u
  X
  1  abcde
  2  fghij
  3  klmno
  t
  u
  X
  1  abcde
  2  fghij
  3  klmno
  t
  u
  X
  1  abcde
  2  fghij
  3  klmno
  t
  u
  X
  1  abcde
  2  fghij
  3  klmno
  t
  u
  X
  1  abcde
  2  fghij
  3  klmno
  t
  u
  X
  1  abcde
  2  fghij
  3  klmno
  t
  u
  X
  1  abcde
  2  fghij
  3  klmno
  t
  u
  X
  1  abcde
  2  fghij
  3  klmno
  t
  u
  X
  1  abcde
  2  fghij
  3  klmno
  t
  u
  X
  1  abcde
  2  fghij
  3  klmno
  t
  u
  X
  1  abcde
  2  fghij
  3  klmno
  t
  u
  X
  1  abcde
  2  fghij
  3  klmno
  t
  u
  X
  1  abcde
  2  fghij
  3  klmno
  t
  u
  X
  1  abcde
  2  fghij
  3  klmno
  t
  u
  X
  1  abcde
  2  fghij
  3  klmno
  t
  u
  X
  1  abcde
  2  fghij
  3  klmno
  t
  u
  X
  1  abcde
  2  fghij
  3  klmno
  t
  u
  X
===DONE===
