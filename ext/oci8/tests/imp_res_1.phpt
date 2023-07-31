--TEST--
Oracle Database 12c Implicit Result Sets: basic test
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
    "drop table imp_res_1_tab_1",
    "create table imp_res_1_tab_1 (c1 number, c2 varchar2(10))",
    "insert into imp_res_1_tab_1 values (1, 'abcde')",
    "insert into imp_res_1_tab_1 values (2, 'fghij')",
    "insert into imp_res_1_tab_1 values (3, 'klmno')",

    "drop table imp_res_1_tab_2",
    "create table imp_res_1_tab_2 (c3 varchar2(1))",
    "insert into imp_res_1_tab_2 values ('t')",
    "insert into imp_res_1_tab_2 values ('u')",
    "insert into imp_res_1_tab_2 values ('v')",

    "create or replace procedure imp_res_1_proc as
      c1 sys_refcursor;
    begin
      open c1 for select * from imp_res_1_tab_1 order by 1;
      dbms_sql.return_result(c1);

      open c1 for select * from imp_res_1_tab_2 where rownum < 3 order by 1;
      dbms_sql.return_result(c1);

      open c1 for select 99 from dual;
      dbms_sql.return_result (c1);

      open c1 for select NULL, 'Z' from dual;
      dbms_sql.return_result (c1);

      open c1 for select * from imp_res_1_tab_1 order by 1;
      dbms_sql.return_result(c1);
    end;"
);

oci8_test_sql_execute($c, $stmtarray);

// Run Test

echo "Test 1 - oci_fetch_assoc\n";
$s = oci_parse($c, "begin imp_res_1_proc(); end;");
oci_execute($s);
while (($row = oci_fetch_assoc($s)) != false)
    var_dump($row);

echo "\nTest 2 - oci_fetch_object\n";
$s = oci_parse($c, "begin imp_res_1_proc(); end;");
oci_execute($s);
while (($row = oci_fetch_object($s)) != false)
    var_dump($row);

echo "\nTest 3 - oci_fetch_row\n";
$s = oci_parse($c, "begin imp_res_1_proc(); end;");
oci_execute($s);
while (($row = oci_fetch_row($s)) != false)
    var_dump($row);

echo "\nTest 4 - oci_fetch_array(OCI_ASSOC+OCI_RETURN_NULLS)\n";
$s = oci_parse($c, "begin imp_res_1_proc(); end;");
oci_execute($s);
while (($row = oci_fetch_array($s, OCI_ASSOC+OCI_RETURN_NULLS)) != false)
    var_dump($row);

echo "\nTest 5 - oci_fetch_array(OCI_ASSOC)\n";
$s = oci_parse($c, "begin imp_res_1_proc(); end;");
oci_execute($s);
while (($row = oci_fetch_array($s, OCI_ASSOC)) != false)
    var_dump($row);

echo "\nTest 6 - oci_fetch_array(OCI_NUM)\n";
$s = oci_parse($c, "begin imp_res_1_proc(); end;");
oci_execute($s);
while (($row = oci_fetch_array($s, OCI_NUM)) != false)
    var_dump($row);

echo "\nTest 7 - oci_fetch_array(OCI_BOTH)\n";
$s = oci_parse($c, "begin imp_res_1_proc(); end;");
oci_execute($s);
while (($row = oci_fetch_array($s, OCI_BOTH)) != false)
    var_dump($row);

echo "\nTest 8 - oci_fetch_array(OCI_BOTH+OCI_RETURN_NULLS)\n";
$s = oci_parse($c, "begin imp_res_1_proc(); end;");
oci_execute($s);
while (($row = oci_fetch_array($s, OCI_BOTH+OCI_RETURN_NULLS)) != false)
    var_dump($row);

// Clean up

$stmtarray = array(
    "drop procedure imp_res_1_proc",
    "drop table imp_res_1_tab_1",
    "drop table imp_res_1_tab_2"
);

oci8_test_sql_execute($c, $stmtarray);

?>
--EXPECTF--
Test 1 - oci_fetch_assoc
array(2) {
  ["C1"]=>
  string(1) "1"
  ["C2"]=>
  string(5) "abcde"
}
array(2) {
  ["C1"]=>
  string(1) "2"
  ["C2"]=>
  string(5) "fghij"
}
array(2) {
  ["C1"]=>
  string(1) "3"
  ["C2"]=>
  string(5) "klmno"
}
array(1) {
  ["C3"]=>
  string(1) "t"
}
array(1) {
  ["C3"]=>
  string(1) "u"
}
array(1) {
  [99]=>
  string(2) "99"
}
array(2) {
  ["NULL"]=>
  NULL
  ["'Z'"]=>
  string(1) "Z"
}
array(2) {
  ["C1"]=>
  string(1) "1"
  ["C2"]=>
  string(5) "abcde"
}
array(2) {
  ["C1"]=>
  string(1) "2"
  ["C2"]=>
  string(5) "fghij"
}
array(2) {
  ["C1"]=>
  string(1) "3"
  ["C2"]=>
  string(5) "klmno"
}

Test 2 - oci_fetch_object
object(stdClass)#%d (2) {
  ["C1"]=>
  string(1) "1"
  ["C2"]=>
  string(5) "abcde"
}
object(stdClass)#%d (2) {
  ["C1"]=>
  string(1) "2"
  ["C2"]=>
  string(5) "fghij"
}
object(stdClass)#%d (2) {
  ["C1"]=>
  string(1) "3"
  ["C2"]=>
  string(5) "klmno"
}
object(stdClass)#%d (1) {
  ["C3"]=>
  string(1) "t"
}
object(stdClass)#%d (1) {
  ["C3"]=>
  string(1) "u"
}
object(stdClass)#%d (1) {
  [99]=>
  string(2) "99"
}
object(stdClass)#%d (2) {
  ["NULL"]=>
  NULL
  ["'Z'"]=>
  string(1) "Z"
}
object(stdClass)#%d (2) {
  ["C1"]=>
  string(1) "1"
  ["C2"]=>
  string(5) "abcde"
}
object(stdClass)#%d (2) {
  ["C1"]=>
  string(1) "2"
  ["C2"]=>
  string(5) "fghij"
}
object(stdClass)#%d (2) {
  ["C1"]=>
  string(1) "3"
  ["C2"]=>
  string(5) "klmno"
}

Test 3 - oci_fetch_row
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
  string(2) "99"
}
array(2) {
  [0]=>
  NULL
  [1]=>
  string(1) "Z"
}
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

Test 4 - oci_fetch_array(OCI_ASSOC+OCI_RETURN_NULLS)
array(2) {
  ["C1"]=>
  string(1) "1"
  ["C2"]=>
  string(5) "abcde"
}
array(2) {
  ["C1"]=>
  string(1) "2"
  ["C2"]=>
  string(5) "fghij"
}
array(2) {
  ["C1"]=>
  string(1) "3"
  ["C2"]=>
  string(5) "klmno"
}
array(1) {
  ["C3"]=>
  string(1) "t"
}
array(1) {
  ["C3"]=>
  string(1) "u"
}
array(1) {
  [99]=>
  string(2) "99"
}
array(2) {
  ["NULL"]=>
  NULL
  ["'Z'"]=>
  string(1) "Z"
}
array(2) {
  ["C1"]=>
  string(1) "1"
  ["C2"]=>
  string(5) "abcde"
}
array(2) {
  ["C1"]=>
  string(1) "2"
  ["C2"]=>
  string(5) "fghij"
}
array(2) {
  ["C1"]=>
  string(1) "3"
  ["C2"]=>
  string(5) "klmno"
}

Test 5 - oci_fetch_array(OCI_ASSOC)
array(2) {
  ["C1"]=>
  string(1) "1"
  ["C2"]=>
  string(5) "abcde"
}
array(2) {
  ["C1"]=>
  string(1) "2"
  ["C2"]=>
  string(5) "fghij"
}
array(2) {
  ["C1"]=>
  string(1) "3"
  ["C2"]=>
  string(5) "klmno"
}
array(1) {
  ["C3"]=>
  string(1) "t"
}
array(1) {
  ["C3"]=>
  string(1) "u"
}
array(1) {
  [99]=>
  string(2) "99"
}
array(1) {
  ["'Z'"]=>
  string(1) "Z"
}
array(2) {
  ["C1"]=>
  string(1) "1"
  ["C2"]=>
  string(5) "abcde"
}
array(2) {
  ["C1"]=>
  string(1) "2"
  ["C2"]=>
  string(5) "fghij"
}
array(2) {
  ["C1"]=>
  string(1) "3"
  ["C2"]=>
  string(5) "klmno"
}

Test 6 - oci_fetch_array(OCI_NUM)
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
  string(2) "99"
}
array(1) {
  [1]=>
  string(1) "Z"
}
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

Test 7 - oci_fetch_array(OCI_BOTH)
array(4) {
  [0]=>
  string(1) "1"
  ["C1"]=>
  string(1) "1"
  [1]=>
  string(5) "abcde"
  ["C2"]=>
  string(5) "abcde"
}
array(4) {
  [0]=>
  string(1) "2"
  ["C1"]=>
  string(1) "2"
  [1]=>
  string(5) "fghij"
  ["C2"]=>
  string(5) "fghij"
}
array(4) {
  [0]=>
  string(1) "3"
  ["C1"]=>
  string(1) "3"
  [1]=>
  string(5) "klmno"
  ["C2"]=>
  string(5) "klmno"
}
array(2) {
  [0]=>
  string(1) "t"
  ["C3"]=>
  string(1) "t"
}
array(2) {
  [0]=>
  string(1) "u"
  ["C3"]=>
  string(1) "u"
}
array(2) {
  [0]=>
  string(2) "99"
  [99]=>
  string(2) "99"
}
array(2) {
  [1]=>
  string(1) "Z"
  ["'Z'"]=>
  string(1) "Z"
}
array(4) {
  [0]=>
  string(1) "1"
  ["C1"]=>
  string(1) "1"
  [1]=>
  string(5) "abcde"
  ["C2"]=>
  string(5) "abcde"
}
array(4) {
  [0]=>
  string(1) "2"
  ["C1"]=>
  string(1) "2"
  [1]=>
  string(5) "fghij"
  ["C2"]=>
  string(5) "fghij"
}
array(4) {
  [0]=>
  string(1) "3"
  ["C1"]=>
  string(1) "3"
  [1]=>
  string(5) "klmno"
  ["C2"]=>
  string(5) "klmno"
}

Test 8 - oci_fetch_array(OCI_BOTH+OCI_RETURN_NULLS)
array(4) {
  [0]=>
  string(1) "1"
  ["C1"]=>
  string(1) "1"
  [1]=>
  string(5) "abcde"
  ["C2"]=>
  string(5) "abcde"
}
array(4) {
  [0]=>
  string(1) "2"
  ["C1"]=>
  string(1) "2"
  [1]=>
  string(5) "fghij"
  ["C2"]=>
  string(5) "fghij"
}
array(4) {
  [0]=>
  string(1) "3"
  ["C1"]=>
  string(1) "3"
  [1]=>
  string(5) "klmno"
  ["C2"]=>
  string(5) "klmno"
}
array(2) {
  [0]=>
  string(1) "t"
  ["C3"]=>
  string(1) "t"
}
array(2) {
  [0]=>
  string(1) "u"
  ["C3"]=>
  string(1) "u"
}
array(2) {
  [0]=>
  string(2) "99"
  [99]=>
  string(2) "99"
}
array(4) {
  [0]=>
  NULL
  ["NULL"]=>
  NULL
  [1]=>
  string(1) "Z"
  ["'Z'"]=>
  string(1) "Z"
}
array(4) {
  [0]=>
  string(1) "1"
  ["C1"]=>
  string(1) "1"
  [1]=>
  string(5) "abcde"
  ["C2"]=>
  string(5) "abcde"
}
array(4) {
  [0]=>
  string(1) "2"
  ["C1"]=>
  string(1) "2"
  [1]=>
  string(5) "fghij"
  ["C2"]=>
  string(5) "fghij"
}
array(4) {
  [0]=>
  string(1) "3"
  ["C1"]=>
  string(1) "3"
  [1]=>
  string(5) "klmno"
  ["C2"]=>
  string(5) "klmno"
}
