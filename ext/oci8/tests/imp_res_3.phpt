--TEST--
Oracle Database 12c Implicit Result Sets: bigger data size
--EXTENSIONS--
oci8
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require __DIR__.'/skipif.inc';
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

require __DIR__.'/connect.inc';

// Initialization

$stmtarray = array(
    "drop table imp_res_3_tab_1",
    "create table imp_res_3_tab_1 (c1 number, c2 varchar2(10))",
    "insert into imp_res_3_tab_1 values (1, 'a')",
    "insert into imp_res_3_tab_1 values (2, 'f')",

    "drop table imp_res_3_tab_2",
    "create table imp_res_3_tab_2 (c3 varchar2(1))",
    "insert into imp_res_3_tab_2 values ('t')",
    "insert into imp_res_3_tab_2 values ('u')",
    "insert into imp_res_3_tab_2 values ('v')",
    "insert into imp_res_3_tab_2 values ('w')",

    "create or replace procedure imp_res_3_proc as
      c1 sys_refcursor;
      i pls_integer;
    begin
      for i in 1..30 loop  -- if this value is too big for Oracle's open_cursors, calling imp_res_3_proc() can fail with ORA-1000
        open c1 for select t1.*, t2.*, t3.*, t4.*, t5.*
            from imp_res_3_tab_1 t1, imp_res_3_tab_1 t2, imp_res_3_tab_1 t3,
                 imp_res_3_tab_1 t4, imp_res_3_tab_1 t5 order by 1,3,5,7,9,2,4,6,8,10;
        dbms_sql.return_result(c1);
        open c1 for select c2 from imp_res_3_tab_1 order by 1;
        dbms_sql.return_result(c1);
        open c1 for select * from imp_res_3_tab_2 order by 1;
        dbms_sql.return_result(c1);
        open c1 for select * from dual;
        dbms_sql.return_result (c1);
      end loop;
    end;"
);

oci8_test_sql_execute($c, $stmtarray);

// Run Test

echo "Test 1\n";

$s = oci_parse($c, "begin imp_res_3_proc(); end;");
oci_execute($s);

while (($row = oci_fetch_array($s, OCI_NUM+OCI_RETURN_NULLS)) != false) {
    foreach ($row as $item) {
        echo "  ".$item;
    }
    echo "\n";
}

// Clean up

$stmtarray = array(
    "drop procedure imp_res_3_proc",
    "drop table imp_res_3_tab_1",
    "drop table imp_res_3_tab_2"
);

oci8_test_sql_execute($c, $stmtarray);

?>
--EXPECT--
Test 1
  1  a  1  a  1  a  1  a  1  a
  1  a  1  a  1  a  1  a  2  f
  1  a  1  a  1  a  2  f  1  a
  1  a  1  a  1  a  2  f  2  f
  1  a  1  a  2  f  1  a  1  a
  1  a  1  a  2  f  1  a  2  f
  1  a  1  a  2  f  2  f  1  a
  1  a  1  a  2  f  2  f  2  f
  1  a  2  f  1  a  1  a  1  a
  1  a  2  f  1  a  1  a  2  f
  1  a  2  f  1  a  2  f  1  a
  1  a  2  f  1  a  2  f  2  f
  1  a  2  f  2  f  1  a  1  a
  1  a  2  f  2  f  1  a  2  f
  1  a  2  f  2  f  2  f  1  a
  1  a  2  f  2  f  2  f  2  f
  2  f  1  a  1  a  1  a  1  a
  2  f  1  a  1  a  1  a  2  f
  2  f  1  a  1  a  2  f  1  a
  2  f  1  a  1  a  2  f  2  f
  2  f  1  a  2  f  1  a  1  a
  2  f  1  a  2  f  1  a  2  f
  2  f  1  a  2  f  2  f  1  a
  2  f  1  a  2  f  2  f  2  f
  2  f  2  f  1  a  1  a  1  a
  2  f  2  f  1  a  1  a  2  f
  2  f  2  f  1  a  2  f  1  a
  2  f  2  f  1  a  2  f  2  f
  2  f  2  f  2  f  1  a  1  a
  2  f  2  f  2  f  1  a  2  f
  2  f  2  f  2  f  2  f  1  a
  2  f  2  f  2  f  2  f  2  f
  a
  f
  t
  u
  v
  w
  X
  1  a  1  a  1  a  1  a  1  a
  1  a  1  a  1  a  1  a  2  f
  1  a  1  a  1  a  2  f  1  a
  1  a  1  a  1  a  2  f  2  f
  1  a  1  a  2  f  1  a  1  a
  1  a  1  a  2  f  1  a  2  f
  1  a  1  a  2  f  2  f  1  a
  1  a  1  a  2  f  2  f  2  f
  1  a  2  f  1  a  1  a  1  a
  1  a  2  f  1  a  1  a  2  f
  1  a  2  f  1  a  2  f  1  a
  1  a  2  f  1  a  2  f  2  f
  1  a  2  f  2  f  1  a  1  a
  1  a  2  f  2  f  1  a  2  f
  1  a  2  f  2  f  2  f  1  a
  1  a  2  f  2  f  2  f  2  f
  2  f  1  a  1  a  1  a  1  a
  2  f  1  a  1  a  1  a  2  f
  2  f  1  a  1  a  2  f  1  a
  2  f  1  a  1  a  2  f  2  f
  2  f  1  a  2  f  1  a  1  a
  2  f  1  a  2  f  1  a  2  f
  2  f  1  a  2  f  2  f  1  a
  2  f  1  a  2  f  2  f  2  f
  2  f  2  f  1  a  1  a  1  a
  2  f  2  f  1  a  1  a  2  f
  2  f  2  f  1  a  2  f  1  a
  2  f  2  f  1  a  2  f  2  f
  2  f  2  f  2  f  1  a  1  a
  2  f  2  f  2  f  1  a  2  f
  2  f  2  f  2  f  2  f  1  a
  2  f  2  f  2  f  2  f  2  f
  a
  f
  t
  u
  v
  w
  X
  1  a  1  a  1  a  1  a  1  a
  1  a  1  a  1  a  1  a  2  f
  1  a  1  a  1  a  2  f  1  a
  1  a  1  a  1  a  2  f  2  f
  1  a  1  a  2  f  1  a  1  a
  1  a  1  a  2  f  1  a  2  f
  1  a  1  a  2  f  2  f  1  a
  1  a  1  a  2  f  2  f  2  f
  1  a  2  f  1  a  1  a  1  a
  1  a  2  f  1  a  1  a  2  f
  1  a  2  f  1  a  2  f  1  a
  1  a  2  f  1  a  2  f  2  f
  1  a  2  f  2  f  1  a  1  a
  1  a  2  f  2  f  1  a  2  f
  1  a  2  f  2  f  2  f  1  a
  1  a  2  f  2  f  2  f  2  f
  2  f  1  a  1  a  1  a  1  a
  2  f  1  a  1  a  1  a  2  f
  2  f  1  a  1  a  2  f  1  a
  2  f  1  a  1  a  2  f  2  f
  2  f  1  a  2  f  1  a  1  a
  2  f  1  a  2  f  1  a  2  f
  2  f  1  a  2  f  2  f  1  a
  2  f  1  a  2  f  2  f  2  f
  2  f  2  f  1  a  1  a  1  a
  2  f  2  f  1  a  1  a  2  f
  2  f  2  f  1  a  2  f  1  a
  2  f  2  f  1  a  2  f  2  f
  2  f  2  f  2  f  1  a  1  a
  2  f  2  f  2  f  1  a  2  f
  2  f  2  f  2  f  2  f  1  a
  2  f  2  f  2  f  2  f  2  f
  a
  f
  t
  u
  v
  w
  X
  1  a  1  a  1  a  1  a  1  a
  1  a  1  a  1  a  1  a  2  f
  1  a  1  a  1  a  2  f  1  a
  1  a  1  a  1  a  2  f  2  f
  1  a  1  a  2  f  1  a  1  a
  1  a  1  a  2  f  1  a  2  f
  1  a  1  a  2  f  2  f  1  a
  1  a  1  a  2  f  2  f  2  f
  1  a  2  f  1  a  1  a  1  a
  1  a  2  f  1  a  1  a  2  f
  1  a  2  f  1  a  2  f  1  a
  1  a  2  f  1  a  2  f  2  f
  1  a  2  f  2  f  1  a  1  a
  1  a  2  f  2  f  1  a  2  f
  1  a  2  f  2  f  2  f  1  a
  1  a  2  f  2  f  2  f  2  f
  2  f  1  a  1  a  1  a  1  a
  2  f  1  a  1  a  1  a  2  f
  2  f  1  a  1  a  2  f  1  a
  2  f  1  a  1  a  2  f  2  f
  2  f  1  a  2  f  1  a  1  a
  2  f  1  a  2  f  1  a  2  f
  2  f  1  a  2  f  2  f  1  a
  2  f  1  a  2  f  2  f  2  f
  2  f  2  f  1  a  1  a  1  a
  2  f  2  f  1  a  1  a  2  f
  2  f  2  f  1  a  2  f  1  a
  2  f  2  f  1  a  2  f  2  f
  2  f  2  f  2  f  1  a  1  a
  2  f  2  f  2  f  1  a  2  f
  2  f  2  f  2  f  2  f  1  a
  2  f  2  f  2  f  2  f  2  f
  a
  f
  t
  u
  v
  w
  X
  1  a  1  a  1  a  1  a  1  a
  1  a  1  a  1  a  1  a  2  f
  1  a  1  a  1  a  2  f  1  a
  1  a  1  a  1  a  2  f  2  f
  1  a  1  a  2  f  1  a  1  a
  1  a  1  a  2  f  1  a  2  f
  1  a  1  a  2  f  2  f  1  a
  1  a  1  a  2  f  2  f  2  f
  1  a  2  f  1  a  1  a  1  a
  1  a  2  f  1  a  1  a  2  f
  1  a  2  f  1  a  2  f  1  a
  1  a  2  f  1  a  2  f  2  f
  1  a  2  f  2  f  1  a  1  a
  1  a  2  f  2  f  1  a  2  f
  1  a  2  f  2  f  2  f  1  a
  1  a  2  f  2  f  2  f  2  f
  2  f  1  a  1  a  1  a  1  a
  2  f  1  a  1  a  1  a  2  f
  2  f  1  a  1  a  2  f  1  a
  2  f  1  a  1  a  2  f  2  f
  2  f  1  a  2  f  1  a  1  a
  2  f  1  a  2  f  1  a  2  f
  2  f  1  a  2  f  2  f  1  a
  2  f  1  a  2  f  2  f  2  f
  2  f  2  f  1  a  1  a  1  a
  2  f  2  f  1  a  1  a  2  f
  2  f  2  f  1  a  2  f  1  a
  2  f  2  f  1  a  2  f  2  f
  2  f  2  f  2  f  1  a  1  a
  2  f  2  f  2  f  1  a  2  f
  2  f  2  f  2  f  2  f  1  a
  2  f  2  f  2  f  2  f  2  f
  a
  f
  t
  u
  v
  w
  X
  1  a  1  a  1  a  1  a  1  a
  1  a  1  a  1  a  1  a  2  f
  1  a  1  a  1  a  2  f  1  a
  1  a  1  a  1  a  2  f  2  f
  1  a  1  a  2  f  1  a  1  a
  1  a  1  a  2  f  1  a  2  f
  1  a  1  a  2  f  2  f  1  a
  1  a  1  a  2  f  2  f  2  f
  1  a  2  f  1  a  1  a  1  a
  1  a  2  f  1  a  1  a  2  f
  1  a  2  f  1  a  2  f  1  a
  1  a  2  f  1  a  2  f  2  f
  1  a  2  f  2  f  1  a  1  a
  1  a  2  f  2  f  1  a  2  f
  1  a  2  f  2  f  2  f  1  a
  1  a  2  f  2  f  2  f  2  f
  2  f  1  a  1  a  1  a  1  a
  2  f  1  a  1  a  1  a  2  f
  2  f  1  a  1  a  2  f  1  a
  2  f  1  a  1  a  2  f  2  f
  2  f  1  a  2  f  1  a  1  a
  2  f  1  a  2  f  1  a  2  f
  2  f  1  a  2  f  2  f  1  a
  2  f  1  a  2  f  2  f  2  f
  2  f  2  f  1  a  1  a  1  a
  2  f  2  f  1  a  1  a  2  f
  2  f  2  f  1  a  2  f  1  a
  2  f  2  f  1  a  2  f  2  f
  2  f  2  f  2  f  1  a  1  a
  2  f  2  f  2  f  1  a  2  f
  2  f  2  f  2  f  2  f  1  a
  2  f  2  f  2  f  2  f  2  f
  a
  f
  t
  u
  v
  w
  X
  1  a  1  a  1  a  1  a  1  a
  1  a  1  a  1  a  1  a  2  f
  1  a  1  a  1  a  2  f  1  a
  1  a  1  a  1  a  2  f  2  f
  1  a  1  a  2  f  1  a  1  a
  1  a  1  a  2  f  1  a  2  f
  1  a  1  a  2  f  2  f  1  a
  1  a  1  a  2  f  2  f  2  f
  1  a  2  f  1  a  1  a  1  a
  1  a  2  f  1  a  1  a  2  f
  1  a  2  f  1  a  2  f  1  a
  1  a  2  f  1  a  2  f  2  f
  1  a  2  f  2  f  1  a  1  a
  1  a  2  f  2  f  1  a  2  f
  1  a  2  f  2  f  2  f  1  a
  1  a  2  f  2  f  2  f  2  f
  2  f  1  a  1  a  1  a  1  a
  2  f  1  a  1  a  1  a  2  f
  2  f  1  a  1  a  2  f  1  a
  2  f  1  a  1  a  2  f  2  f
  2  f  1  a  2  f  1  a  1  a
  2  f  1  a  2  f  1  a  2  f
  2  f  1  a  2  f  2  f  1  a
  2  f  1  a  2  f  2  f  2  f
  2  f  2  f  1  a  1  a  1  a
  2  f  2  f  1  a  1  a  2  f
  2  f  2  f  1  a  2  f  1  a
  2  f  2  f  1  a  2  f  2  f
  2  f  2  f  2  f  1  a  1  a
  2  f  2  f  2  f  1  a  2  f
  2  f  2  f  2  f  2  f  1  a
  2  f  2  f  2  f  2  f  2  f
  a
  f
  t
  u
  v
  w
  X
  1  a  1  a  1  a  1  a  1  a
  1  a  1  a  1  a  1  a  2  f
  1  a  1  a  1  a  2  f  1  a
  1  a  1  a  1  a  2  f  2  f
  1  a  1  a  2  f  1  a  1  a
  1  a  1  a  2  f  1  a  2  f
  1  a  1  a  2  f  2  f  1  a
  1  a  1  a  2  f  2  f  2  f
  1  a  2  f  1  a  1  a  1  a
  1  a  2  f  1  a  1  a  2  f
  1  a  2  f  1  a  2  f  1  a
  1  a  2  f  1  a  2  f  2  f
  1  a  2  f  2  f  1  a  1  a
  1  a  2  f  2  f  1  a  2  f
  1  a  2  f  2  f  2  f  1  a
  1  a  2  f  2  f  2  f  2  f
  2  f  1  a  1  a  1  a  1  a
  2  f  1  a  1  a  1  a  2  f
  2  f  1  a  1  a  2  f  1  a
  2  f  1  a  1  a  2  f  2  f
  2  f  1  a  2  f  1  a  1  a
  2  f  1  a  2  f  1  a  2  f
  2  f  1  a  2  f  2  f  1  a
  2  f  1  a  2  f  2  f  2  f
  2  f  2  f  1  a  1  a  1  a
  2  f  2  f  1  a  1  a  2  f
  2  f  2  f  1  a  2  f  1  a
  2  f  2  f  1  a  2  f  2  f
  2  f  2  f  2  f  1  a  1  a
  2  f  2  f  2  f  1  a  2  f
  2  f  2  f  2  f  2  f  1  a
  2  f  2  f  2  f  2  f  2  f
  a
  f
  t
  u
  v
  w
  X
  1  a  1  a  1  a  1  a  1  a
  1  a  1  a  1  a  1  a  2  f
  1  a  1  a  1  a  2  f  1  a
  1  a  1  a  1  a  2  f  2  f
  1  a  1  a  2  f  1  a  1  a
  1  a  1  a  2  f  1  a  2  f
  1  a  1  a  2  f  2  f  1  a
  1  a  1  a  2  f  2  f  2  f
  1  a  2  f  1  a  1  a  1  a
  1  a  2  f  1  a  1  a  2  f
  1  a  2  f  1  a  2  f  1  a
  1  a  2  f  1  a  2  f  2  f
  1  a  2  f  2  f  1  a  1  a
  1  a  2  f  2  f  1  a  2  f
  1  a  2  f  2  f  2  f  1  a
  1  a  2  f  2  f  2  f  2  f
  2  f  1  a  1  a  1  a  1  a
  2  f  1  a  1  a  1  a  2  f
  2  f  1  a  1  a  2  f  1  a
  2  f  1  a  1  a  2  f  2  f
  2  f  1  a  2  f  1  a  1  a
  2  f  1  a  2  f  1  a  2  f
  2  f  1  a  2  f  2  f  1  a
  2  f  1  a  2  f  2  f  2  f
  2  f  2  f  1  a  1  a  1  a
  2  f  2  f  1  a  1  a  2  f
  2  f  2  f  1  a  2  f  1  a
  2  f  2  f  1  a  2  f  2  f
  2  f  2  f  2  f  1  a  1  a
  2  f  2  f  2  f  1  a  2  f
  2  f  2  f  2  f  2  f  1  a
  2  f  2  f  2  f  2  f  2  f
  a
  f
  t
  u
  v
  w
  X
  1  a  1  a  1  a  1  a  1  a
  1  a  1  a  1  a  1  a  2  f
  1  a  1  a  1  a  2  f  1  a
  1  a  1  a  1  a  2  f  2  f
  1  a  1  a  2  f  1  a  1  a
  1  a  1  a  2  f  1  a  2  f
  1  a  1  a  2  f  2  f  1  a
  1  a  1  a  2  f  2  f  2  f
  1  a  2  f  1  a  1  a  1  a
  1  a  2  f  1  a  1  a  2  f
  1  a  2  f  1  a  2  f  1  a
  1  a  2  f  1  a  2  f  2  f
  1  a  2  f  2  f  1  a  1  a
  1  a  2  f  2  f  1  a  2  f
  1  a  2  f  2  f  2  f  1  a
  1  a  2  f  2  f  2  f  2  f
  2  f  1  a  1  a  1  a  1  a
  2  f  1  a  1  a  1  a  2  f
  2  f  1  a  1  a  2  f  1  a
  2  f  1  a  1  a  2  f  2  f
  2  f  1  a  2  f  1  a  1  a
  2  f  1  a  2  f  1  a  2  f
  2  f  1  a  2  f  2  f  1  a
  2  f  1  a  2  f  2  f  2  f
  2  f  2  f  1  a  1  a  1  a
  2  f  2  f  1  a  1  a  2  f
  2  f  2  f  1  a  2  f  1  a
  2  f  2  f  1  a  2  f  2  f
  2  f  2  f  2  f  1  a  1  a
  2  f  2  f  2  f  1  a  2  f
  2  f  2  f  2  f  2  f  1  a
  2  f  2  f  2  f  2  f  2  f
  a
  f
  t
  u
  v
  w
  X
  1  a  1  a  1  a  1  a  1  a
  1  a  1  a  1  a  1  a  2  f
  1  a  1  a  1  a  2  f  1  a
  1  a  1  a  1  a  2  f  2  f
  1  a  1  a  2  f  1  a  1  a
  1  a  1  a  2  f  1  a  2  f
  1  a  1  a  2  f  2  f  1  a
  1  a  1  a  2  f  2  f  2  f
  1  a  2  f  1  a  1  a  1  a
  1  a  2  f  1  a  1  a  2  f
  1  a  2  f  1  a  2  f  1  a
  1  a  2  f  1  a  2  f  2  f
  1  a  2  f  2  f  1  a  1  a
  1  a  2  f  2  f  1  a  2  f
  1  a  2  f  2  f  2  f  1  a
  1  a  2  f  2  f  2  f  2  f
  2  f  1  a  1  a  1  a  1  a
  2  f  1  a  1  a  1  a  2  f
  2  f  1  a  1  a  2  f  1  a
  2  f  1  a  1  a  2  f  2  f
  2  f  1  a  2  f  1  a  1  a
  2  f  1  a  2  f  1  a  2  f
  2  f  1  a  2  f  2  f  1  a
  2  f  1  a  2  f  2  f  2  f
  2  f  2  f  1  a  1  a  1  a
  2  f  2  f  1  a  1  a  2  f
  2  f  2  f  1  a  2  f  1  a
  2  f  2  f  1  a  2  f  2  f
  2  f  2  f  2  f  1  a  1  a
  2  f  2  f  2  f  1  a  2  f
  2  f  2  f  2  f  2  f  1  a
  2  f  2  f  2  f  2  f  2  f
  a
  f
  t
  u
  v
  w
  X
  1  a  1  a  1  a  1  a  1  a
  1  a  1  a  1  a  1  a  2  f
  1  a  1  a  1  a  2  f  1  a
  1  a  1  a  1  a  2  f  2  f
  1  a  1  a  2  f  1  a  1  a
  1  a  1  a  2  f  1  a  2  f
  1  a  1  a  2  f  2  f  1  a
  1  a  1  a  2  f  2  f  2  f
  1  a  2  f  1  a  1  a  1  a
  1  a  2  f  1  a  1  a  2  f
  1  a  2  f  1  a  2  f  1  a
  1  a  2  f  1  a  2  f  2  f
  1  a  2  f  2  f  1  a  1  a
  1  a  2  f  2  f  1  a  2  f
  1  a  2  f  2  f  2  f  1  a
  1  a  2  f  2  f  2  f  2  f
  2  f  1  a  1  a  1  a  1  a
  2  f  1  a  1  a  1  a  2  f
  2  f  1  a  1  a  2  f  1  a
  2  f  1  a  1  a  2  f  2  f
  2  f  1  a  2  f  1  a  1  a
  2  f  1  a  2  f  1  a  2  f
  2  f  1  a  2  f  2  f  1  a
  2  f  1  a  2  f  2  f  2  f
  2  f  2  f  1  a  1  a  1  a
  2  f  2  f  1  a  1  a  2  f
  2  f  2  f  1  a  2  f  1  a
  2  f  2  f  1  a  2  f  2  f
  2  f  2  f  2  f  1  a  1  a
  2  f  2  f  2  f  1  a  2  f
  2  f  2  f  2  f  2  f  1  a
  2  f  2  f  2  f  2  f  2  f
  a
  f
  t
  u
  v
  w
  X
  1  a  1  a  1  a  1  a  1  a
  1  a  1  a  1  a  1  a  2  f
  1  a  1  a  1  a  2  f  1  a
  1  a  1  a  1  a  2  f  2  f
  1  a  1  a  2  f  1  a  1  a
  1  a  1  a  2  f  1  a  2  f
  1  a  1  a  2  f  2  f  1  a
  1  a  1  a  2  f  2  f  2  f
  1  a  2  f  1  a  1  a  1  a
  1  a  2  f  1  a  1  a  2  f
  1  a  2  f  1  a  2  f  1  a
  1  a  2  f  1  a  2  f  2  f
  1  a  2  f  2  f  1  a  1  a
  1  a  2  f  2  f  1  a  2  f
  1  a  2  f  2  f  2  f  1  a
  1  a  2  f  2  f  2  f  2  f
  2  f  1  a  1  a  1  a  1  a
  2  f  1  a  1  a  1  a  2  f
  2  f  1  a  1  a  2  f  1  a
  2  f  1  a  1  a  2  f  2  f
  2  f  1  a  2  f  1  a  1  a
  2  f  1  a  2  f  1  a  2  f
  2  f  1  a  2  f  2  f  1  a
  2  f  1  a  2  f  2  f  2  f
  2  f  2  f  1  a  1  a  1  a
  2  f  2  f  1  a  1  a  2  f
  2  f  2  f  1  a  2  f  1  a
  2  f  2  f  1  a  2  f  2  f
  2  f  2  f  2  f  1  a  1  a
  2  f  2  f  2  f  1  a  2  f
  2  f  2  f  2  f  2  f  1  a
  2  f  2  f  2  f  2  f  2  f
  a
  f
  t
  u
  v
  w
  X
  1  a  1  a  1  a  1  a  1  a
  1  a  1  a  1  a  1  a  2  f
  1  a  1  a  1  a  2  f  1  a
  1  a  1  a  1  a  2  f  2  f
  1  a  1  a  2  f  1  a  1  a
  1  a  1  a  2  f  1  a  2  f
  1  a  1  a  2  f  2  f  1  a
  1  a  1  a  2  f  2  f  2  f
  1  a  2  f  1  a  1  a  1  a
  1  a  2  f  1  a  1  a  2  f
  1  a  2  f  1  a  2  f  1  a
  1  a  2  f  1  a  2  f  2  f
  1  a  2  f  2  f  1  a  1  a
  1  a  2  f  2  f  1  a  2  f
  1  a  2  f  2  f  2  f  1  a
  1  a  2  f  2  f  2  f  2  f
  2  f  1  a  1  a  1  a  1  a
  2  f  1  a  1  a  1  a  2  f
  2  f  1  a  1  a  2  f  1  a
  2  f  1  a  1  a  2  f  2  f
  2  f  1  a  2  f  1  a  1  a
  2  f  1  a  2  f  1  a  2  f
  2  f  1  a  2  f  2  f  1  a
  2  f  1  a  2  f  2  f  2  f
  2  f  2  f  1  a  1  a  1  a
  2  f  2  f  1  a  1  a  2  f
  2  f  2  f  1  a  2  f  1  a
  2  f  2  f  1  a  2  f  2  f
  2  f  2  f  2  f  1  a  1  a
  2  f  2  f  2  f  1  a  2  f
  2  f  2  f  2  f  2  f  1  a
  2  f  2  f  2  f  2  f  2  f
  a
  f
  t
  u
  v
  w
  X
  1  a  1  a  1  a  1  a  1  a
  1  a  1  a  1  a  1  a  2  f
  1  a  1  a  1  a  2  f  1  a
  1  a  1  a  1  a  2  f  2  f
  1  a  1  a  2  f  1  a  1  a
  1  a  1  a  2  f  1  a  2  f
  1  a  1  a  2  f  2  f  1  a
  1  a  1  a  2  f  2  f  2  f
  1  a  2  f  1  a  1  a  1  a
  1  a  2  f  1  a  1  a  2  f
  1  a  2  f  1  a  2  f  1  a
  1  a  2  f  1  a  2  f  2  f
  1  a  2  f  2  f  1  a  1  a
  1  a  2  f  2  f  1  a  2  f
  1  a  2  f  2  f  2  f  1  a
  1  a  2  f  2  f  2  f  2  f
  2  f  1  a  1  a  1  a  1  a
  2  f  1  a  1  a  1  a  2  f
  2  f  1  a  1  a  2  f  1  a
  2  f  1  a  1  a  2  f  2  f
  2  f  1  a  2  f  1  a  1  a
  2  f  1  a  2  f  1  a  2  f
  2  f  1  a  2  f  2  f  1  a
  2  f  1  a  2  f  2  f  2  f
  2  f  2  f  1  a  1  a  1  a
  2  f  2  f  1  a  1  a  2  f
  2  f  2  f  1  a  2  f  1  a
  2  f  2  f  1  a  2  f  2  f
  2  f  2  f  2  f  1  a  1  a
  2  f  2  f  2  f  1  a  2  f
  2  f  2  f  2  f  2  f  1  a
  2  f  2  f  2  f  2  f  2  f
  a
  f
  t
  u
  v
  w
  X
  1  a  1  a  1  a  1  a  1  a
  1  a  1  a  1  a  1  a  2  f
  1  a  1  a  1  a  2  f  1  a
  1  a  1  a  1  a  2  f  2  f
  1  a  1  a  2  f  1  a  1  a
  1  a  1  a  2  f  1  a  2  f
  1  a  1  a  2  f  2  f  1  a
  1  a  1  a  2  f  2  f  2  f
  1  a  2  f  1  a  1  a  1  a
  1  a  2  f  1  a  1  a  2  f
  1  a  2  f  1  a  2  f  1  a
  1  a  2  f  1  a  2  f  2  f
  1  a  2  f  2  f  1  a  1  a
  1  a  2  f  2  f  1  a  2  f
  1  a  2  f  2  f  2  f  1  a
  1  a  2  f  2  f  2  f  2  f
  2  f  1  a  1  a  1  a  1  a
  2  f  1  a  1  a  1  a  2  f
  2  f  1  a  1  a  2  f  1  a
  2  f  1  a  1  a  2  f  2  f
  2  f  1  a  2  f  1  a  1  a
  2  f  1  a  2  f  1  a  2  f
  2  f  1  a  2  f  2  f  1  a
  2  f  1  a  2  f  2  f  2  f
  2  f  2  f  1  a  1  a  1  a
  2  f  2  f  1  a  1  a  2  f
  2  f  2  f  1  a  2  f  1  a
  2  f  2  f  1  a  2  f  2  f
  2  f  2  f  2  f  1  a  1  a
  2  f  2  f  2  f  1  a  2  f
  2  f  2  f  2  f  2  f  1  a
  2  f  2  f  2  f  2  f  2  f
  a
  f
  t
  u
  v
  w
  X
  1  a  1  a  1  a  1  a  1  a
  1  a  1  a  1  a  1  a  2  f
  1  a  1  a  1  a  2  f  1  a
  1  a  1  a  1  a  2  f  2  f
  1  a  1  a  2  f  1  a  1  a
  1  a  1  a  2  f  1  a  2  f
  1  a  1  a  2  f  2  f  1  a
  1  a  1  a  2  f  2  f  2  f
  1  a  2  f  1  a  1  a  1  a
  1  a  2  f  1  a  1  a  2  f
  1  a  2  f  1  a  2  f  1  a
  1  a  2  f  1  a  2  f  2  f
  1  a  2  f  2  f  1  a  1  a
  1  a  2  f  2  f  1  a  2  f
  1  a  2  f  2  f  2  f  1  a
  1  a  2  f  2  f  2  f  2  f
  2  f  1  a  1  a  1  a  1  a
  2  f  1  a  1  a  1  a  2  f
  2  f  1  a  1  a  2  f  1  a
  2  f  1  a  1  a  2  f  2  f
  2  f  1  a  2  f  1  a  1  a
  2  f  1  a  2  f  1  a  2  f
  2  f  1  a  2  f  2  f  1  a
  2  f  1  a  2  f  2  f  2  f
  2  f  2  f  1  a  1  a  1  a
  2  f  2  f  1  a  1  a  2  f
  2  f  2  f  1  a  2  f  1  a
  2  f  2  f  1  a  2  f  2  f
  2  f  2  f  2  f  1  a  1  a
  2  f  2  f  2  f  1  a  2  f
  2  f  2  f  2  f  2  f  1  a
  2  f  2  f  2  f  2  f  2  f
  a
  f
  t
  u
  v
  w
  X
  1  a  1  a  1  a  1  a  1  a
  1  a  1  a  1  a  1  a  2  f
  1  a  1  a  1  a  2  f  1  a
  1  a  1  a  1  a  2  f  2  f
  1  a  1  a  2  f  1  a  1  a
  1  a  1  a  2  f  1  a  2  f
  1  a  1  a  2  f  2  f  1  a
  1  a  1  a  2  f  2  f  2  f
  1  a  2  f  1  a  1  a  1  a
  1  a  2  f  1  a  1  a  2  f
  1  a  2  f  1  a  2  f  1  a
  1  a  2  f  1  a  2  f  2  f
  1  a  2  f  2  f  1  a  1  a
  1  a  2  f  2  f  1  a  2  f
  1  a  2  f  2  f  2  f  1  a
  1  a  2  f  2  f  2  f  2  f
  2  f  1  a  1  a  1  a  1  a
  2  f  1  a  1  a  1  a  2  f
  2  f  1  a  1  a  2  f  1  a
  2  f  1  a  1  a  2  f  2  f
  2  f  1  a  2  f  1  a  1  a
  2  f  1  a  2  f  1  a  2  f
  2  f  1  a  2  f  2  f  1  a
  2  f  1  a  2  f  2  f  2  f
  2  f  2  f  1  a  1  a  1  a
  2  f  2  f  1  a  1  a  2  f
  2  f  2  f  1  a  2  f  1  a
  2  f  2  f  1  a  2  f  2  f
  2  f  2  f  2  f  1  a  1  a
  2  f  2  f  2  f  1  a  2  f
  2  f  2  f  2  f  2  f  1  a
  2  f  2  f  2  f  2  f  2  f
  a
  f
  t
  u
  v
  w
  X
  1  a  1  a  1  a  1  a  1  a
  1  a  1  a  1  a  1  a  2  f
  1  a  1  a  1  a  2  f  1  a
  1  a  1  a  1  a  2  f  2  f
  1  a  1  a  2  f  1  a  1  a
  1  a  1  a  2  f  1  a  2  f
  1  a  1  a  2  f  2  f  1  a
  1  a  1  a  2  f  2  f  2  f
  1  a  2  f  1  a  1  a  1  a
  1  a  2  f  1  a  1  a  2  f
  1  a  2  f  1  a  2  f  1  a
  1  a  2  f  1  a  2  f  2  f
  1  a  2  f  2  f  1  a  1  a
  1  a  2  f  2  f  1  a  2  f
  1  a  2  f  2  f  2  f  1  a
  1  a  2  f  2  f  2  f  2  f
  2  f  1  a  1  a  1  a  1  a
  2  f  1  a  1  a  1  a  2  f
  2  f  1  a  1  a  2  f  1  a
  2  f  1  a  1  a  2  f  2  f
  2  f  1  a  2  f  1  a  1  a
  2  f  1  a  2  f  1  a  2  f
  2  f  1  a  2  f  2  f  1  a
  2  f  1  a  2  f  2  f  2  f
  2  f  2  f  1  a  1  a  1  a
  2  f  2  f  1  a  1  a  2  f
  2  f  2  f  1  a  2  f  1  a
  2  f  2  f  1  a  2  f  2  f
  2  f  2  f  2  f  1  a  1  a
  2  f  2  f  2  f  1  a  2  f
  2  f  2  f  2  f  2  f  1  a
  2  f  2  f  2  f  2  f  2  f
  a
  f
  t
  u
  v
  w
  X
  1  a  1  a  1  a  1  a  1  a
  1  a  1  a  1  a  1  a  2  f
  1  a  1  a  1  a  2  f  1  a
  1  a  1  a  1  a  2  f  2  f
  1  a  1  a  2  f  1  a  1  a
  1  a  1  a  2  f  1  a  2  f
  1  a  1  a  2  f  2  f  1  a
  1  a  1  a  2  f  2  f  2  f
  1  a  2  f  1  a  1  a  1  a
  1  a  2  f  1  a  1  a  2  f
  1  a  2  f  1  a  2  f  1  a
  1  a  2  f  1  a  2  f  2  f
  1  a  2  f  2  f  1  a  1  a
  1  a  2  f  2  f  1  a  2  f
  1  a  2  f  2  f  2  f  1  a
  1  a  2  f  2  f  2  f  2  f
  2  f  1  a  1  a  1  a  1  a
  2  f  1  a  1  a  1  a  2  f
  2  f  1  a  1  a  2  f  1  a
  2  f  1  a  1  a  2  f  2  f
  2  f  1  a  2  f  1  a  1  a
  2  f  1  a  2  f  1  a  2  f
  2  f  1  a  2  f  2  f  1  a
  2  f  1  a  2  f  2  f  2  f
  2  f  2  f  1  a  1  a  1  a
  2  f  2  f  1  a  1  a  2  f
  2  f  2  f  1  a  2  f  1  a
  2  f  2  f  1  a  2  f  2  f
  2  f  2  f  2  f  1  a  1  a
  2  f  2  f  2  f  1  a  2  f
  2  f  2  f  2  f  2  f  1  a
  2  f  2  f  2  f  2  f  2  f
  a
  f
  t
  u
  v
  w
  X
  1  a  1  a  1  a  1  a  1  a
  1  a  1  a  1  a  1  a  2  f
  1  a  1  a  1  a  2  f  1  a
  1  a  1  a  1  a  2  f  2  f
  1  a  1  a  2  f  1  a  1  a
  1  a  1  a  2  f  1  a  2  f
  1  a  1  a  2  f  2  f  1  a
  1  a  1  a  2  f  2  f  2  f
  1  a  2  f  1  a  1  a  1  a
  1  a  2  f  1  a  1  a  2  f
  1  a  2  f  1  a  2  f  1  a
  1  a  2  f  1  a  2  f  2  f
  1  a  2  f  2  f  1  a  1  a
  1  a  2  f  2  f  1  a  2  f
  1  a  2  f  2  f  2  f  1  a
  1  a  2  f  2  f  2  f  2  f
  2  f  1  a  1  a  1  a  1  a
  2  f  1  a  1  a  1  a  2  f
  2  f  1  a  1  a  2  f  1  a
  2  f  1  a  1  a  2  f  2  f
  2  f  1  a  2  f  1  a  1  a
  2  f  1  a  2  f  1  a  2  f
  2  f  1  a  2  f  2  f  1  a
  2  f  1  a  2  f  2  f  2  f
  2  f  2  f  1  a  1  a  1  a
  2  f  2  f  1  a  1  a  2  f
  2  f  2  f  1  a  2  f  1  a
  2  f  2  f  1  a  2  f  2  f
  2  f  2  f  2  f  1  a  1  a
  2  f  2  f  2  f  1  a  2  f
  2  f  2  f  2  f  2  f  1  a
  2  f  2  f  2  f  2  f  2  f
  a
  f
  t
  u
  v
  w
  X
  1  a  1  a  1  a  1  a  1  a
  1  a  1  a  1  a  1  a  2  f
  1  a  1  a  1  a  2  f  1  a
  1  a  1  a  1  a  2  f  2  f
  1  a  1  a  2  f  1  a  1  a
  1  a  1  a  2  f  1  a  2  f
  1  a  1  a  2  f  2  f  1  a
  1  a  1  a  2  f  2  f  2  f
  1  a  2  f  1  a  1  a  1  a
  1  a  2  f  1  a  1  a  2  f
  1  a  2  f  1  a  2  f  1  a
  1  a  2  f  1  a  2  f  2  f
  1  a  2  f  2  f  1  a  1  a
  1  a  2  f  2  f  1  a  2  f
  1  a  2  f  2  f  2  f  1  a
  1  a  2  f  2  f  2  f  2  f
  2  f  1  a  1  a  1  a  1  a
  2  f  1  a  1  a  1  a  2  f
  2  f  1  a  1  a  2  f  1  a
  2  f  1  a  1  a  2  f  2  f
  2  f  1  a  2  f  1  a  1  a
  2  f  1  a  2  f  1  a  2  f
  2  f  1  a  2  f  2  f  1  a
  2  f  1  a  2  f  2  f  2  f
  2  f  2  f  1  a  1  a  1  a
  2  f  2  f  1  a  1  a  2  f
  2  f  2  f  1  a  2  f  1  a
  2  f  2  f  1  a  2  f  2  f
  2  f  2  f  2  f  1  a  1  a
  2  f  2  f  2  f  1  a  2  f
  2  f  2  f  2  f  2  f  1  a
  2  f  2  f  2  f  2  f  2  f
  a
  f
  t
  u
  v
  w
  X
  1  a  1  a  1  a  1  a  1  a
  1  a  1  a  1  a  1  a  2  f
  1  a  1  a  1  a  2  f  1  a
  1  a  1  a  1  a  2  f  2  f
  1  a  1  a  2  f  1  a  1  a
  1  a  1  a  2  f  1  a  2  f
  1  a  1  a  2  f  2  f  1  a
  1  a  1  a  2  f  2  f  2  f
  1  a  2  f  1  a  1  a  1  a
  1  a  2  f  1  a  1  a  2  f
  1  a  2  f  1  a  2  f  1  a
  1  a  2  f  1  a  2  f  2  f
  1  a  2  f  2  f  1  a  1  a
  1  a  2  f  2  f  1  a  2  f
  1  a  2  f  2  f  2  f  1  a
  1  a  2  f  2  f  2  f  2  f
  2  f  1  a  1  a  1  a  1  a
  2  f  1  a  1  a  1  a  2  f
  2  f  1  a  1  a  2  f  1  a
  2  f  1  a  1  a  2  f  2  f
  2  f  1  a  2  f  1  a  1  a
  2  f  1  a  2  f  1  a  2  f
  2  f  1  a  2  f  2  f  1  a
  2  f  1  a  2  f  2  f  2  f
  2  f  2  f  1  a  1  a  1  a
  2  f  2  f  1  a  1  a  2  f
  2  f  2  f  1  a  2  f  1  a
  2  f  2  f  1  a  2  f  2  f
  2  f  2  f  2  f  1  a  1  a
  2  f  2  f  2  f  1  a  2  f
  2  f  2  f  2  f  2  f  1  a
  2  f  2  f  2  f  2  f  2  f
  a
  f
  t
  u
  v
  w
  X
  1  a  1  a  1  a  1  a  1  a
  1  a  1  a  1  a  1  a  2  f
  1  a  1  a  1  a  2  f  1  a
  1  a  1  a  1  a  2  f  2  f
  1  a  1  a  2  f  1  a  1  a
  1  a  1  a  2  f  1  a  2  f
  1  a  1  a  2  f  2  f  1  a
  1  a  1  a  2  f  2  f  2  f
  1  a  2  f  1  a  1  a  1  a
  1  a  2  f  1  a  1  a  2  f
  1  a  2  f  1  a  2  f  1  a
  1  a  2  f  1  a  2  f  2  f
  1  a  2  f  2  f  1  a  1  a
  1  a  2  f  2  f  1  a  2  f
  1  a  2  f  2  f  2  f  1  a
  1  a  2  f  2  f  2  f  2  f
  2  f  1  a  1  a  1  a  1  a
  2  f  1  a  1  a  1  a  2  f
  2  f  1  a  1  a  2  f  1  a
  2  f  1  a  1  a  2  f  2  f
  2  f  1  a  2  f  1  a  1  a
  2  f  1  a  2  f  1  a  2  f
  2  f  1  a  2  f  2  f  1  a
  2  f  1  a  2  f  2  f  2  f
  2  f  2  f  1  a  1  a  1  a
  2  f  2  f  1  a  1  a  2  f
  2  f  2  f  1  a  2  f  1  a
  2  f  2  f  1  a  2  f  2  f
  2  f  2  f  2  f  1  a  1  a
  2  f  2  f  2  f  1  a  2  f
  2  f  2  f  2  f  2  f  1  a
  2  f  2  f  2  f  2  f  2  f
  a
  f
  t
  u
  v
  w
  X
  1  a  1  a  1  a  1  a  1  a
  1  a  1  a  1  a  1  a  2  f
  1  a  1  a  1  a  2  f  1  a
  1  a  1  a  1  a  2  f  2  f
  1  a  1  a  2  f  1  a  1  a
  1  a  1  a  2  f  1  a  2  f
  1  a  1  a  2  f  2  f  1  a
  1  a  1  a  2  f  2  f  2  f
  1  a  2  f  1  a  1  a  1  a
  1  a  2  f  1  a  1  a  2  f
  1  a  2  f  1  a  2  f  1  a
  1  a  2  f  1  a  2  f  2  f
  1  a  2  f  2  f  1  a  1  a
  1  a  2  f  2  f  1  a  2  f
  1  a  2  f  2  f  2  f  1  a
  1  a  2  f  2  f  2  f  2  f
  2  f  1  a  1  a  1  a  1  a
  2  f  1  a  1  a  1  a  2  f
  2  f  1  a  1  a  2  f  1  a
  2  f  1  a  1  a  2  f  2  f
  2  f  1  a  2  f  1  a  1  a
  2  f  1  a  2  f  1  a  2  f
  2  f  1  a  2  f  2  f  1  a
  2  f  1  a  2  f  2  f  2  f
  2  f  2  f  1  a  1  a  1  a
  2  f  2  f  1  a  1  a  2  f
  2  f  2  f  1  a  2  f  1  a
  2  f  2  f  1  a  2  f  2  f
  2  f  2  f  2  f  1  a  1  a
  2  f  2  f  2  f  1  a  2  f
  2  f  2  f  2  f  2  f  1  a
  2  f  2  f  2  f  2  f  2  f
  a
  f
  t
  u
  v
  w
  X
  1  a  1  a  1  a  1  a  1  a
  1  a  1  a  1  a  1  a  2  f
  1  a  1  a  1  a  2  f  1  a
  1  a  1  a  1  a  2  f  2  f
  1  a  1  a  2  f  1  a  1  a
  1  a  1  a  2  f  1  a  2  f
  1  a  1  a  2  f  2  f  1  a
  1  a  1  a  2  f  2  f  2  f
  1  a  2  f  1  a  1  a  1  a
  1  a  2  f  1  a  1  a  2  f
  1  a  2  f  1  a  2  f  1  a
  1  a  2  f  1  a  2  f  2  f
  1  a  2  f  2  f  1  a  1  a
  1  a  2  f  2  f  1  a  2  f
  1  a  2  f  2  f  2  f  1  a
  1  a  2  f  2  f  2  f  2  f
  2  f  1  a  1  a  1  a  1  a
  2  f  1  a  1  a  1  a  2  f
  2  f  1  a  1  a  2  f  1  a
  2  f  1  a  1  a  2  f  2  f
  2  f  1  a  2  f  1  a  1  a
  2  f  1  a  2  f  1  a  2  f
  2  f  1  a  2  f  2  f  1  a
  2  f  1  a  2  f  2  f  2  f
  2  f  2  f  1  a  1  a  1  a
  2  f  2  f  1  a  1  a  2  f
  2  f  2  f  1  a  2  f  1  a
  2  f  2  f  1  a  2  f  2  f
  2  f  2  f  2  f  1  a  1  a
  2  f  2  f  2  f  1  a  2  f
  2  f  2  f  2  f  2  f  1  a
  2  f  2  f  2  f  2  f  2  f
  a
  f
  t
  u
  v
  w
  X
  1  a  1  a  1  a  1  a  1  a
  1  a  1  a  1  a  1  a  2  f
  1  a  1  a  1  a  2  f  1  a
  1  a  1  a  1  a  2  f  2  f
  1  a  1  a  2  f  1  a  1  a
  1  a  1  a  2  f  1  a  2  f
  1  a  1  a  2  f  2  f  1  a
  1  a  1  a  2  f  2  f  2  f
  1  a  2  f  1  a  1  a  1  a
  1  a  2  f  1  a  1  a  2  f
  1  a  2  f  1  a  2  f  1  a
  1  a  2  f  1  a  2  f  2  f
  1  a  2  f  2  f  1  a  1  a
  1  a  2  f  2  f  1  a  2  f
  1  a  2  f  2  f  2  f  1  a
  1  a  2  f  2  f  2  f  2  f
  2  f  1  a  1  a  1  a  1  a
  2  f  1  a  1  a  1  a  2  f
  2  f  1  a  1  a  2  f  1  a
  2  f  1  a  1  a  2  f  2  f
  2  f  1  a  2  f  1  a  1  a
  2  f  1  a  2  f  1  a  2  f
  2  f  1  a  2  f  2  f  1  a
  2  f  1  a  2  f  2  f  2  f
  2  f  2  f  1  a  1  a  1  a
  2  f  2  f  1  a  1  a  2  f
  2  f  2  f  1  a  2  f  1  a
  2  f  2  f  1  a  2  f  2  f
  2  f  2  f  2  f  1  a  1  a
  2  f  2  f  2  f  1  a  2  f
  2  f  2  f  2  f  2  f  1  a
  2  f  2  f  2  f  2  f  2  f
  a
  f
  t
  u
  v
  w
  X
  1  a  1  a  1  a  1  a  1  a
  1  a  1  a  1  a  1  a  2  f
  1  a  1  a  1  a  2  f  1  a
  1  a  1  a  1  a  2  f  2  f
  1  a  1  a  2  f  1  a  1  a
  1  a  1  a  2  f  1  a  2  f
  1  a  1  a  2  f  2  f  1  a
  1  a  1  a  2  f  2  f  2  f
  1  a  2  f  1  a  1  a  1  a
  1  a  2  f  1  a  1  a  2  f
  1  a  2  f  1  a  2  f  1  a
  1  a  2  f  1  a  2  f  2  f
  1  a  2  f  2  f  1  a  1  a
  1  a  2  f  2  f  1  a  2  f
  1  a  2  f  2  f  2  f  1  a
  1  a  2  f  2  f  2  f  2  f
  2  f  1  a  1  a  1  a  1  a
  2  f  1  a  1  a  1  a  2  f
  2  f  1  a  1  a  2  f  1  a
  2  f  1  a  1  a  2  f  2  f
  2  f  1  a  2  f  1  a  1  a
  2  f  1  a  2  f  1  a  2  f
  2  f  1  a  2  f  2  f  1  a
  2  f  1  a  2  f  2  f  2  f
  2  f  2  f  1  a  1  a  1  a
  2  f  2  f  1  a  1  a  2  f
  2  f  2  f  1  a  2  f  1  a
  2  f  2  f  1  a  2  f  2  f
  2  f  2  f  2  f  1  a  1  a
  2  f  2  f  2  f  1  a  2  f
  2  f  2  f  2  f  2  f  1  a
  2  f  2  f  2  f  2  f  2  f
  a
  f
  t
  u
  v
  w
  X
  1  a  1  a  1  a  1  a  1  a
  1  a  1  a  1  a  1  a  2  f
  1  a  1  a  1  a  2  f  1  a
  1  a  1  a  1  a  2  f  2  f
  1  a  1  a  2  f  1  a  1  a
  1  a  1  a  2  f  1  a  2  f
  1  a  1  a  2  f  2  f  1  a
  1  a  1  a  2  f  2  f  2  f
  1  a  2  f  1  a  1  a  1  a
  1  a  2  f  1  a  1  a  2  f
  1  a  2  f  1  a  2  f  1  a
  1  a  2  f  1  a  2  f  2  f
  1  a  2  f  2  f  1  a  1  a
  1  a  2  f  2  f  1  a  2  f
  1  a  2  f  2  f  2  f  1  a
  1  a  2  f  2  f  2  f  2  f
  2  f  1  a  1  a  1  a  1  a
  2  f  1  a  1  a  1  a  2  f
  2  f  1  a  1  a  2  f  1  a
  2  f  1  a  1  a  2  f  2  f
  2  f  1  a  2  f  1  a  1  a
  2  f  1  a  2  f  1  a  2  f
  2  f  1  a  2  f  2  f  1  a
  2  f  1  a  2  f  2  f  2  f
  2  f  2  f  1  a  1  a  1  a
  2  f  2  f  1  a  1  a  2  f
  2  f  2  f  1  a  2  f  1  a
  2  f  2  f  1  a  2  f  2  f
  2  f  2  f  2  f  1  a  1  a
  2  f  2  f  2  f  1  a  2  f
  2  f  2  f  2  f  2  f  1  a
  2  f  2  f  2  f  2  f  2  f
  a
  f
  t
  u
  v
  w
  X
  1  a  1  a  1  a  1  a  1  a
  1  a  1  a  1  a  1  a  2  f
  1  a  1  a  1  a  2  f  1  a
  1  a  1  a  1  a  2  f  2  f
  1  a  1  a  2  f  1  a  1  a
  1  a  1  a  2  f  1  a  2  f
  1  a  1  a  2  f  2  f  1  a
  1  a  1  a  2  f  2  f  2  f
  1  a  2  f  1  a  1  a  1  a
  1  a  2  f  1  a  1  a  2  f
  1  a  2  f  1  a  2  f  1  a
  1  a  2  f  1  a  2  f  2  f
  1  a  2  f  2  f  1  a  1  a
  1  a  2  f  2  f  1  a  2  f
  1  a  2  f  2  f  2  f  1  a
  1  a  2  f  2  f  2  f  2  f
  2  f  1  a  1  a  1  a  1  a
  2  f  1  a  1  a  1  a  2  f
  2  f  1  a  1  a  2  f  1  a
  2  f  1  a  1  a  2  f  2  f
  2  f  1  a  2  f  1  a  1  a
  2  f  1  a  2  f  1  a  2  f
  2  f  1  a  2  f  2  f  1  a
  2  f  1  a  2  f  2  f  2  f
  2  f  2  f  1  a  1  a  1  a
  2  f  2  f  1  a  1  a  2  f
  2  f  2  f  1  a  2  f  1  a
  2  f  2  f  1  a  2  f  2  f
  2  f  2  f  2  f  1  a  1  a
  2  f  2  f  2  f  1  a  2  f
  2  f  2  f  2  f  2  f  1  a
  2  f  2  f  2  f  2  f  2  f
  a
  f
  t
  u
  v
  w
  X
