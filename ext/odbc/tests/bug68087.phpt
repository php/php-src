--TEST--
odbc_exec(): Getting accurate date data from query
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php

include 'config.inc';

$id_1_date = '2014-09-23';
$id_2_date = '2014-09-24';

$conn = odbc_connect($dsn, $user, $pass);

@odbc_exec($conn, 'CREATE DATABASE odbcTEST');

odbc_exec($conn, 'CREATE TABLE FOO (ID INT, VARCHAR_COL VARCHAR(100), DATE_COL DATE)');

odbc_exec($conn, "INSERT INTO FOO(ID, VARCHAR_COL, DATE_COL) VALUES (1, 'hello', '$id_1_date')");
odbc_exec($conn, "INSERT INTO FOO(ID, VARCHAR_COL, DATE_COL) VALUES (2, 'helloagain', '$id_2_date')");

$res = odbc_exec($conn, 'SELECT * FROM FOO ORDER BY ID ASC');

while(odbc_fetch_row($res)) {
    $id = odbc_result($res, "ID");
    $varchar_col = odbc_result($res, "VARCHAR_COL");
    $date = odbc_result($res, "DATE_COL");

    if ($id == 1) {
        if ($date != $id_1_date) {
            print "Date_1 mismatched\n";
        } else {
            print "Date_1 matched\n";
        }
    } else {
        if ($date != $id_2_date) {
            print "Date_2 mismatched\n";
        } else {
            print "Date_2 matched\n";
        }
    }
}

?>
--EXPECT--
Date_1 matched
Date_2 matched
--CLEAN--
<?php
include 'config.inc';

$conn = odbc_connect($dsn, $user, $pass);

odbc_exec($conn, 'DROP TABLE FOO');
odbc_exec($conn, 'DROP DATABASE odbcTEST');

?>
