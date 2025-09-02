--TEST--
odbc_exec(): Getting accurate date data from query
--EXTENSIONS--
odbc
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php

include 'config.inc';

$id_1_date = '2014-09-23';
$id_2_date = '2014-09-24';

$conn = odbc_connect($dsn, $user, $pass);

odbc_exec($conn, 'CREATE TABLE bug68087 (ID INT, VARCHAR_COL VARCHAR(100), DATE_COL DATE)');

odbc_exec($conn, "INSERT INTO bug68087(ID, VARCHAR_COL, DATE_COL) VALUES (1, 'hello', '$id_1_date'), (2, 'helloagain', '$id_2_date')");

$res = odbc_exec($conn, 'SELECT * FROM bug68087 ORDER BY ID ASC');

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
--CLEAN--
<?php
include 'config.inc';

$conn = odbc_connect($dsn, $user, $pass);

odbc_exec($conn, 'DROP TABLE bug68087');

?>
--EXPECT--
Date_1 matched
Date_2 matched
