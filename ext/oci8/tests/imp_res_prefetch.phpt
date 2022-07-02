--TEST--
Oracle Database 12c Implicit Result Sets: basic test
--EXTENSIONS--
oci8
--SKIPIF--
<?php
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

$stmtarray = array(
    "drop table imp_res_prefetch_tab_1",
    "create table imp_res_prefetch_tab_1 (c1 number, c2 varchar2(10))",
    "insert into imp_res_prefetch_tab_1 values (1, 'abcde')",
    "insert into imp_res_prefetch_tab_1 values (2, 'fghij')",
    "insert into imp_res_prefetch_tab_1 values (3, 'klmno')",

    "drop table imp_res_prefetch_tab_2",
    "create table imp_res_prefetch_tab_2 (c3 varchar2(1))",
    "insert into imp_res_prefetch_tab_2 values ('t')",
    "insert into imp_res_prefetch_tab_2 values ('u')",
    "insert into imp_res_prefetch_tab_2 values ('v')",

    "create or replace procedure imp_res_prefetch_proc as
      c1 sys_refcursor;
    begin
      open c1 for select * from imp_res_prefetch_tab_1 order by 1;
      dbms_sql.return_result(c1);

      open c1 for select * from imp_res_prefetch_tab_2 order by 1;
      dbms_sql.return_result(c1);
    end;"
);

oci8_test_sql_execute($c, $stmtarray);

// Run Test

echo "Test 1 - prefetch 0\n";
$s = oci_parse($c, "begin imp_res_prefetch_proc(); end;");
oci_execute($s);
var_dump(oci_set_prefetch($s, 0));
while (($row = oci_fetch_row($s)) != false)
    var_dump($row);

echo "\nTest 1 - prefetch 1\n";
$s = oci_parse($c, "begin imp_res_prefetch_proc(); end;");
oci_execute($s);
var_dump(oci_set_prefetch($s, 1));
while (($row = oci_fetch_row($s)) != false)
    var_dump($row);

echo "\nTest 1 - prefetch 2\n";
$s = oci_parse($c, "begin imp_res_prefetch_proc(); end;");
oci_execute($s);
var_dump(oci_set_prefetch($s, 2));
while (($row = oci_fetch_row($s)) != false)
    var_dump($row);

// Clean up

$stmtarray = array(
    "drop procedure imp_res_prefetch_proc",
    "drop table imp_res_prefetch_tab_1",
    "drop table imp_res_prefetch_tab_2"
);

oci8_test_sql_execute($c, $stmtarray);

?>
--EXPECT--
Test 1 - prefetch 0
bool(true)
array(2) {
  [0]=>
  string(1) "1"
  [1]=>
  string(5) "abcde"
}
array(2) {
  [0]=>
  string(1) "2"
  [1]=>
  string(5) "fghij"
}
array(2) {
  [0]=>
  string(1) "3"
  [1]=>
  string(5) "klmno"
}
array(1) {
  [0]=>
  string(1) "t"
}
array(1) {
  [0]=>
  string(1) "u"
}
array(1) {
  [0]=>
  string(1) "v"
}

Test 1 - prefetch 1
bool(true)
array(2) {
  [0]=>
  string(1) "1"
  [1]=>
  string(5) "abcde"
}
array(2) {
  [0]=>
  string(1) "2"
  [1]=>
  string(5) "fghij"
}
array(2) {
  [0]=>
  string(1) "3"
  [1]=>
  string(5) "klmno"
}
array(1) {
  [0]=>
  string(1) "t"
}
array(1) {
  [0]=>
  string(1) "u"
}
array(1) {
  [0]=>
  string(1) "v"
}

Test 1 - prefetch 2
bool(true)
array(2) {
  [0]=>
  string(1) "1"
  [1]=>
  string(5) "abcde"
}
array(2) {
  [0]=>
  string(1) "2"
  [1]=>
  string(5) "fghij"
}
array(2) {
  [0]=>
  string(1) "3"
  [1]=>
  string(5) "klmno"
}
array(1) {
  [0]=>
  string(1) "t"
}
array(1) {
  [0]=>
  string(1) "u"
}
array(1) {
  [0]=>
  string(1) "v"
}
