--TEST--
Oracle Database 12c Implicit Result Sets: oci_get_implicit_resultset: oci_fetch_all
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

$plsql = "declare
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

$s1 = oci_get_implicit_resultset($s);
oci_fetch_all($s1, $res);
var_dump($res);

$s2 = oci_get_implicit_resultset($s);
oci_fetch_all($s2, $res);
var_dump($res);

$s3 = oci_get_implicit_resultset($s);
oci_fetch_all($s3, $res);
var_dump($res);

echo "\nTest 2\n";
$s = oci_parse($c, $plsql);
oci_execute($s);
while (($s1 = oci_get_implicit_resultset($s))) {
    $r = oci_fetch_all($s1, $res);
    var_dump($res);
}

?>
--EXPECT--
Test 1
array(1) {
  [1]=>
  array(2) {
    [0]=>
    string(1) "1"
    [1]=>
    string(1) "2"
  }
}
array(1) {
  [3]=>
  array(2) {
    [0]=>
    string(1) "3"
    [1]=>
    string(1) "4"
  }
}
array(1) {
  [5]=>
  array(2) {
    [0]=>
    string(1) "5"
    [1]=>
    string(1) "6"
  }
}

Test 2
array(1) {
  [1]=>
  array(2) {
    [0]=>
    string(1) "1"
    [1]=>
    string(1) "2"
  }
}
array(1) {
  [3]=>
  array(2) {
    [0]=>
    string(1) "3"
    [1]=>
    string(1) "4"
  }
}
array(1) {
  [5]=>
  array(2) {
    [0]=>
    string(1) "5"
    [1]=>
    string(1) "6"
  }
}
