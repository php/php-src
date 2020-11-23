--TEST--
Bug #46994 (CLOB size does not update when using CLOB IN OUT param in stored procedure)
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(__DIR__.'/skipif.inc');
?>
--FILE--
<?php

require(__DIR__.'/connect.inc');

// Initialization

$stmtarray = array(
    "create or replace procedure bug46994_proc1(p1 in out nocopy clob) is
         begin
             dbms_lob.trim(p1, 0);
             dbms_lob.writeappend(p1, 26, 'This should be the output.');
         end bug46994_proc1;",
    "create or replace procedure bug46994_proc2(p1 in out nocopy clob) is
         begin
             dbms_lob.trim(p1, 0);
             dbms_lob.writeappend(p1, 37, 'The output should be even longer now.');
         end bug46994_proc2;"
);

oci8_test_sql_execute($c, $stmtarray);

// Run Test

$myclob = oci_new_descriptor($c, OCI_D_LOB);
$myclob->writeTemporary("some data", OCI_TEMP_CLOB);

echo "Test 1\n";

$s = oci_parse($c, "begin bug46994_proc1(:myclob); end;");
oci_bind_by_name($s, ":myclob", $myclob, -1, SQLT_CLOB);
oci_execute($s, OCI_DEFAULT);
var_dump($myclob->load());

echo "Test 2\n";

$s = oci_parse($c, "begin bug46994_proc2(:myclob); end;");
oci_bind_by_name($s, ":myclob", $myclob, -1, SQLT_CLOB);
oci_execute($s, OCI_DEFAULT);
var_dump($myclob->load());

echo "Test 3\n";

$s = oci_parse($c, "begin bug46994_proc1(:myclob); end;");
oci_bind_by_name($s, ":myclob", $myclob, -1, SQLT_CLOB);
oci_execute($s, OCI_DEFAULT);
var_dump($myclob->load());

echo "Test 4\n";

var_dump($myclob->load());  // Use cached size code path

// Cleanup

$stmtarray = array(
    "drop procedure bug46994_proc1",
    "drop procedure bug46994_proc2"
);

oci8_test_sql_execute($c, $stmtarray);

oci_close($c);

?>
--EXPECT--
Test 1
string(26) "This should be the output."
Test 2
string(37) "The output should be even longer now."
Test 3
string(26) "This should be the output."
Test 4
string(26) "This should be the output."
