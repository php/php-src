--TEST--
PL/SQL bind with SQLT_CHR
--SKIPIF--
<?php if (!extension_loaded('oci8')) die ("skip no oci8 extension"); ?>
--FILE--
<?php

require(__DIR__.'/connect.inc');

// Initialization

$stmtarray = array(
    "create or replace procedure bind_sqltchr_proc (msg_in in varchar2, msg_out out varchar2)
    as
    begin
      msg_out := upper(msg_in);
    end;"

);

oci8_test_sql_execute($c, $stmtarray);

// Run Test

echo "Test 1 - PL/SQL IN and OUT variables\n";

$stmt = oci_parse($c, "BEGIN bind_sqltchr_proc(:a, :b); END;");
$msg_in = "Cat got your keyboard?";
oci_bind_by_name($stmt, ":a", $msg_in, -1, SQLT_CHR);
oci_bind_by_name($stmt, ":b", $msg_out, 800, SQLT_CHR);
oci_execute($stmt);
var_dump($msg_in);
var_dump($msg_out);

// Clean up

$stmtarray = array(
    "drop procedure bind_sqltchr_proc"
);

oci8_test_sql_execute($c, $stmtarray);

?>
===DONE===
<?php exit(0); ?>
--EXPECT--
Test 1 - PL/SQL IN and OUT variables
string(22) "Cat got your keyboard?"
string(22) "CAT GOT YOUR KEYBOARD?"
===DONE===
