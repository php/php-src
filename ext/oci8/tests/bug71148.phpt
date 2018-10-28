--TEST--
Bug #71448 (Binding reference overwritten on php7)
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => true);  // test runs on these DBs
require(dirname(__FILE__).'/skipif.inc');
?>
--FILE--
<?php

require(dirname(__FILE__).'/connect.inc');

// Initialize

$stmtarray = array(
    "CREATE OR REPLACE FUNCTION bindfunc(var1 varchar2, var2 varchar2)
       RETURN varchar2
       AS var3 VARCHAR2(20);
       BEGIN
         var3 := CONCAT(var1, var2);
         RETURN var3;
       END;",
    "CREATE OR REPLACE PROCEDURE bindproc(var1 IN string, var2 IN string, var3 IN OUT string) IS
       BEGIN
         var3 := CONCAT(var1, var3);
         var3 := CONCAT(var3, var2);
       END;"
);

oci8_test_sql_execute($c, $stmtarray);

// Run test

function bindvar($stmt, $name, $var)
{
  oci_bind_by_name($stmt, $name, $var);
}

// Test 1: Bind input parameter in a local function
$sql = "select :var1, :var2 from dual";
$cache1 = "INSTR1";
$cache2 = "INSTR2";

echo "Test 1: Bind input parameter in a local function\n";
$stmt = oci_parse($c, $sql);

bindvar($stmt, ':var1', $cache1);
bindvar($stmt, ':var2', $cache2);

oci_execute($stmt);

var_dump(oci_fetch_assoc($stmt));

oci_free_statement($stmt);

// Test 2: Bind output parameter in a local function
$sql = "begin :output1 := 'OUTSTR1'; :output2 := 'OUTSTR2'; end;";
$cache1 = "xxxxxx";
$cache2 = "xxxxxx";

echo "\nTest 2: Bind output parameter in a local function\n";
$stmt = oci_parse($c, $sql);

bindvar($stmt, ':output1', $cache1);
bindvar($stmt, ':output2', $cache2);

oci_execute($stmt);

var_dump($cache1);
var_dump($cache2);

oci_free_statement($stmt);

// Test 3: Bind output parameter within the same scope of execute
$sql = "begin :output1 := 'OUTSTR1'; :output2 := 'OUTSTR2'; end;";
$cache1 = "xxxxxx";
$cache2 = "xxxxxx";

echo "\nTest 3: Bind output parameter within the same scope of execute\n";
$stmt = oci_parse($c, $sql);

oci_bind_by_name($stmt, ":output1", $cache1);
oci_bind_by_name($stmt, ":output2", $cache2);

oci_execute($stmt);

var_dump($cache1);
var_dump($cache2);

oci_free_statement($stmt);

// Test 4: Bind output parameter within the same scope of execute
$sql= "begin :output := bindfunc(:var1, :var2); end;";
$cache1 = "STR1";
$cache2 = "STR2";

echo "\nTest 4: Bind output parameter within the same scope of execute\n";
$stmt = oci_parse($c, $sql);

oci_bind_by_name($stmt, ":var1", $cache1, -1);
oci_bind_by_name($stmt, ":var2", $cache2, -1);
oci_bind_by_name($stmt, ":output", $cache3, 100);

oci_execute($stmt);

var_dump($cache3);

// Test 5: Bind IN OUT parameter in a local function

$sql = "call bindproc(:var1, :var2, :var3)";
$cache1 = 'STR1';
$cache2 = 'STR2';
$cache3 = ' ';

echo "\nTest 5: Bind IN OUT parameter in a local function\n";
$stmt = oci_parse($c, $sql);

bindvar($stmt, ':var1', $cache1);
bindvar($stmt, ':var2', $cache2);
bindvar($stmt, ':var3', $cache3);

oci_execute($stmt);

var_dump($cache1);
var_dump($cache2);
var_dump($cache3);

oci_free_statement($stmt);

// Test 6: Bind IN OUT parameter within the same scope of execute

$sql = "call bindproc(:var1, :var2, :var3)";
$cache1 = 'STR1';
$cache2 = 'STR2';
$cache3 = ' ';

echo "\nTest 6: Bind IN OUT parameter within the same scope of execute\n";
$stmt = oci_parse($c, $sql);

oci_bind_by_name($stmt, ":var1", $cache1, -1);
oci_bind_by_name($stmt, ":var2", $cache2, -1);
oci_bind_by_name($stmt, ":var3", $cache3, 100);

oci_execute($stmt);

var_dump($cache1);
var_dump($cache2);
var_dump($cache3);

// Cleanup

$stmtarray = array(
    "DROP FUNCTION bindfunc",
    "DROP PROCEDURE bindproc"
);

oci8_test_sql_execute($c, $stmtarray);

?>
===DONE===
<?php exit(0); ?>
--EXPECT--
Test 1: Bind input parameter in a local function
array(2) {
  [":VAR1"]=>
  string(6) "INSTR1"
  [":VAR2"]=>
  string(6) "INSTR2"
}

Test 2: Bind output parameter in a local function
string(6) "xxxxxx"
string(6) "xxxxxx"

Test 3: Bind output parameter within the same scope of execute
string(7) "OUTSTR1"
string(7) "OUTSTR2"

Test 4: Bind output parameter within the same scope of execute
string(8) "STR1STR2"

Test 5: Bind IN OUT parameter in a local function
string(4) "STR1"
string(4) "STR2"
string(1) " "

Test 6: Bind IN OUT parameter within the same scope of execute
string(4) "STR1"
string(4) "STR2"
string(9) "STR1 STR2"
===DONE===
