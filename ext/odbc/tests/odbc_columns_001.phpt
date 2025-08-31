--TEST--
odbc_columns(): Basic test
--EXTENSIONS--
odbc
--SKIPIF--
<?php
include 'skipif.inc';

$conn = odbc_connect($dsn, $user, $pass);
$result = @odbc_exec($conn, "SELECT @@Version");
if ($result) {
    $array = odbc_fetch_array($result);
    $info = (string) reset($array);
    if (!str_contains($info, "Microsoft SQL Server")) {
       echo "skip MS SQL specific test";
    }
}
?>
--FILE--
<?php

include 'config.inc';

$conn = odbc_connect($dsn, $user, $pass);
odbc_exec($conn, 'CREATE DATABASE ColumnsTest');
odbc_exec($conn, 'USE ColumnsTest');
odbc_exec($conn, 'CREATE TABLE columns (foo INT NOT NULL, bar VARCHAR(20))');

$res = odbc_columns($conn, "ColumnsTest", "dbo", "columns", "foo");
var_dump(odbc_fetch_array($res));
odbc_free_result($res);

$res = odbc_columns($conn, "ColumnsTest", "dbo", "columns", null);
var_dump(odbc_fetch_array($res));
odbc_free_result($res);

$res = odbc_columns($conn, "ColumnsTest", "dbo", null);
var_dump(odbc_fetch_array($res));
odbc_free_result($res);

$res = odbc_columns($conn, "ColumnsTest", "db%", "columns", "foo");
var_dump(odbc_fetch_array($res));
odbc_free_result($res);

$res = odbc_columns($conn, "ColumnsTest", "dbo", "column_", "bar");
var_dump(odbc_fetch_array($res));
odbc_free_result($res);

$res = odbc_columns($conn, "ColumnsTest", "dbo", "columns", "b%");
var_dump(odbc_fetch_array($res));
odbc_free_result($res);

?>
--CLEAN--
<?php
require 'config.inc';
$conn = odbc_connect($dsn, $user, $pass);
odbc_exec($conn, 'USE ColumnsTest');
odbc_exec($conn, 'DROP TABLE columns');
odbc_exec($conn, 'USE master');
odbc_exec($conn, 'DROP DATABASE ColumnsTest');
?>
--EXPECTF--
array(29) {
  ["TABLE_CAT"]=>
  string(11) "ColumnsTest"
  ["TABLE_SCHEM"]=>
  string(3) "dbo"
  ["TABLE_NAME"]=>
  string(7) "columns"
  ["COLUMN_NAME"]=>
  string(3) "foo"
  ["DATA_TYPE"]=>
  string(1) "4"
  ["TYPE_NAME"]=>
  string(3) "int"
  ["COLUMN_SIZE"]=>
  string(2) "10"
  ["BUFFER_LENGTH"]=>
  string(1) "4"
  ["DECIMAL_DIGITS"]=>
  string(1) "0"
  ["NUM_PREC_RADIX"]=>
  string(2) "10"
  ["NULLABLE"]=>
  string(1) "0"
  ["REMARKS"]=>
  NULL
  ["COLUMN_DEF"]=>
  NULL
  ["SQL_DATA_TYPE"]=>
  string(1) "4"
  ["SQL_DATETIME_SUB"]=>
  NULL
  ["CHAR_OCTET_LENGTH"]=>
  NULL
  ["ORDINAL_POSITION"]=>
  string(1) "1"
  ["IS_NULLABLE"]=>
  string(2) "NO"
  ["SS_IS_SPARSE"]=>
  string(1) "0"
  ["SS_IS_COLUMN_SET"]=>
  string(1) "0"
  ["SS_IS_COMPUTED"]=>
  string(1) "0"
  ["SS_IS_IDENTITY"]=>
  string(1) "0"
  ["SS_UDT_CATALOG_NAME"]=>
  NULL
  ["SS_UDT_SCHEMA_NAME"]=>
  NULL
  ["SS_UDT_ASSEMBLY_TYPE_NAME"]=>
  NULL
  ["SS_XML_SCHEMACOLLECTION_CATALOG_NAME"]=>
  NULL
  ["SS_XML_SCHEMACOLLECTION_SCHEMA_NAME"]=>
  NULL
  ["SS_XML_SCHEMACOLLECTION_NAME"]=>
  NULL
  ["SS_DATA_TYPE"]=>
  string(2) "56"
}
array(29) {
  ["TABLE_CAT"]=>
  string(11) "ColumnsTest"
  ["TABLE_SCHEM"]=>
  string(3) "dbo"
  ["TABLE_NAME"]=>
  string(7) "columns"
  ["COLUMN_NAME"]=>
  string(3) "foo"
  ["DATA_TYPE"]=>
  string(1) "4"
  ["TYPE_NAME"]=>
  string(3) "int"
  ["COLUMN_SIZE"]=>
  string(2) "10"
  ["BUFFER_LENGTH"]=>
  string(1) "4"
  ["DECIMAL_DIGITS"]=>
  string(1) "0"
  ["NUM_PREC_RADIX"]=>
  string(2) "10"
  ["NULLABLE"]=>
  string(1) "0"
  ["REMARKS"]=>
  NULL
  ["COLUMN_DEF"]=>
  NULL
  ["SQL_DATA_TYPE"]=>
  string(1) "4"
  ["SQL_DATETIME_SUB"]=>
  NULL
  ["CHAR_OCTET_LENGTH"]=>
  NULL
  ["ORDINAL_POSITION"]=>
  string(1) "1"
  ["IS_NULLABLE"]=>
  string(2) "NO"
  ["SS_IS_SPARSE"]=>
  string(1) "0"
  ["SS_IS_COLUMN_SET"]=>
  string(1) "0"
  ["SS_IS_COMPUTED"]=>
  string(1) "0"
  ["SS_IS_IDENTITY"]=>
  string(1) "0"
  ["SS_UDT_CATALOG_NAME"]=>
  NULL
  ["SS_UDT_SCHEMA_NAME"]=>
  NULL
  ["SS_UDT_ASSEMBLY_TYPE_NAME"]=>
  NULL
  ["SS_XML_SCHEMACOLLECTION_CATALOG_NAME"]=>
  NULL
  ["SS_XML_SCHEMACOLLECTION_SCHEMA_NAME"]=>
  NULL
  ["SS_XML_SCHEMACOLLECTION_NAME"]=>
  NULL
  ["SS_DATA_TYPE"]=>
  string(2) "56"
}
array(29) {
  ["TABLE_CAT"]=>
  string(11) "ColumnsTest"
  ["TABLE_SCHEM"]=>
  string(3) "dbo"
  ["TABLE_NAME"]=>
  string(7) "columns"
  ["COLUMN_NAME"]=>
  string(3) "foo"
  ["DATA_TYPE"]=>
  string(1) "4"
  ["TYPE_NAME"]=>
  string(3) "int"
  ["COLUMN_SIZE"]=>
  string(2) "10"
  ["BUFFER_LENGTH"]=>
  string(1) "4"
  ["DECIMAL_DIGITS"]=>
  string(1) "0"
  ["NUM_PREC_RADIX"]=>
  string(2) "10"
  ["NULLABLE"]=>
  string(1) "0"
  ["REMARKS"]=>
  NULL
  ["COLUMN_DEF"]=>
  NULL
  ["SQL_DATA_TYPE"]=>
  string(1) "4"
  ["SQL_DATETIME_SUB"]=>
  NULL
  ["CHAR_OCTET_LENGTH"]=>
  NULL
  ["ORDINAL_POSITION"]=>
  string(1) "1"
  ["IS_NULLABLE"]=>
  string(2) "NO"
  ["SS_IS_SPARSE"]=>
  string(1) "0"
  ["SS_IS_COLUMN_SET"]=>
  string(1) "0"
  ["SS_IS_COMPUTED"]=>
  string(1) "0"
  ["SS_IS_IDENTITY"]=>
  string(1) "0"
  ["SS_UDT_CATALOG_NAME"]=>
  NULL
  ["SS_UDT_SCHEMA_NAME"]=>
  NULL
  ["SS_UDT_ASSEMBLY_TYPE_NAME"]=>
  NULL
  ["SS_XML_SCHEMACOLLECTION_CATALOG_NAME"]=>
  NULL
  ["SS_XML_SCHEMACOLLECTION_SCHEMA_NAME"]=>
  NULL
  ["SS_XML_SCHEMACOLLECTION_NAME"]=>
  NULL
  ["SS_DATA_TYPE"]=>
  string(2) "56"
}
array(29) {
  ["TABLE_CAT"]=>
  string(11) "ColumnsTest"
  ["TABLE_SCHEM"]=>
  string(3) "dbo"
  ["TABLE_NAME"]=>
  string(7) "columns"
  ["COLUMN_NAME"]=>
  string(3) "foo"
  ["DATA_TYPE"]=>
  string(1) "4"
  ["TYPE_NAME"]=>
  string(3) "int"
  ["COLUMN_SIZE"]=>
  string(2) "10"
  ["BUFFER_LENGTH"]=>
  string(1) "4"
  ["DECIMAL_DIGITS"]=>
  string(1) "0"
  ["NUM_PREC_RADIX"]=>
  string(2) "10"
  ["NULLABLE"]=>
  string(1) "0"
  ["REMARKS"]=>
  NULL
  ["COLUMN_DEF"]=>
  NULL
  ["SQL_DATA_TYPE"]=>
  string(1) "4"
  ["SQL_DATETIME_SUB"]=>
  NULL
  ["CHAR_OCTET_LENGTH"]=>
  NULL
  ["ORDINAL_POSITION"]=>
  string(1) "1"
  ["IS_NULLABLE"]=>
  string(2) "NO"
  ["SS_IS_SPARSE"]=>
  string(1) "0"
  ["SS_IS_COLUMN_SET"]=>
  string(1) "0"
  ["SS_IS_COMPUTED"]=>
  string(1) "0"
  ["SS_IS_IDENTITY"]=>
  string(1) "0"
  ["SS_UDT_CATALOG_NAME"]=>
  NULL
  ["SS_UDT_SCHEMA_NAME"]=>
  NULL
  ["SS_UDT_ASSEMBLY_TYPE_NAME"]=>
  NULL
  ["SS_XML_SCHEMACOLLECTION_CATALOG_NAME"]=>
  NULL
  ["SS_XML_SCHEMACOLLECTION_SCHEMA_NAME"]=>
  NULL
  ["SS_XML_SCHEMACOLLECTION_NAME"]=>
  NULL
  ["SS_DATA_TYPE"]=>
  string(2) "56"
}
array(29) {
  ["TABLE_CAT"]=>
  string(11) "ColumnsTest"
  ["TABLE_SCHEM"]=>
  string(3) "dbo"
  ["TABLE_NAME"]=>
  string(7) "columns"
  ["COLUMN_NAME"]=>
  string(3) "bar"
  ["DATA_TYPE"]=>
  string(2) "12"
  ["TYPE_NAME"]=>
  string(7) "varchar"
  ["COLUMN_SIZE"]=>
  string(2) "20"
  ["BUFFER_LENGTH"]=>
  string(2) "20"
  ["DECIMAL_DIGITS"]=>
  NULL
  ["NUM_PREC_RADIX"]=>
  NULL
  ["NULLABLE"]=>
  string(1) "1"
  ["REMARKS"]=>
  NULL
  ["COLUMN_DEF"]=>
  NULL
  ["SQL_DATA_TYPE"]=>
  string(2) "12"
  ["SQL_DATETIME_SUB"]=>
  NULL
  ["CHAR_OCTET_LENGTH"]=>
  string(2) "20"
  ["ORDINAL_POSITION"]=>
  string(1) "2"
  ["IS_NULLABLE"]=>
  string(3) "YES"
  ["SS_IS_SPARSE"]=>
  string(1) "0"
  ["SS_IS_COLUMN_SET"]=>
  string(1) "0"
  ["SS_IS_COMPUTED"]=>
  string(1) "0"
  ["SS_IS_IDENTITY"]=>
  string(1) "0"
  ["SS_UDT_CATALOG_NAME"]=>
  NULL
  ["SS_UDT_SCHEMA_NAME"]=>
  NULL
  ["SS_UDT_ASSEMBLY_TYPE_NAME"]=>
  NULL
  ["SS_XML_SCHEMACOLLECTION_CATALOG_NAME"]=>
  NULL
  ["SS_XML_SCHEMACOLLECTION_SCHEMA_NAME"]=>
  NULL
  ["SS_XML_SCHEMACOLLECTION_NAME"]=>
  NULL
  ["SS_DATA_TYPE"]=>
  string(2) "39"
}
array(29) {
  ["TABLE_CAT"]=>
  string(11) "ColumnsTest"
  ["TABLE_SCHEM"]=>
  string(3) "dbo"
  ["TABLE_NAME"]=>
  string(7) "columns"
  ["COLUMN_NAME"]=>
  string(3) "bar"
  ["DATA_TYPE"]=>
  string(2) "12"
  ["TYPE_NAME"]=>
  string(7) "varchar"
  ["COLUMN_SIZE"]=>
  string(2) "20"
  ["BUFFER_LENGTH"]=>
  string(2) "20"
  ["DECIMAL_DIGITS"]=>
  NULL
  ["NUM_PREC_RADIX"]=>
  NULL
  ["NULLABLE"]=>
  string(1) "1"
  ["REMARKS"]=>
  NULL
  ["COLUMN_DEF"]=>
  NULL
  ["SQL_DATA_TYPE"]=>
  string(2) "12"
  ["SQL_DATETIME_SUB"]=>
  NULL
  ["CHAR_OCTET_LENGTH"]=>
  string(2) "20"
  ["ORDINAL_POSITION"]=>
  string(1) "2"
  ["IS_NULLABLE"]=>
  string(3) "YES"
  ["SS_IS_SPARSE"]=>
  string(1) "0"
  ["SS_IS_COLUMN_SET"]=>
  string(1) "0"
  ["SS_IS_COMPUTED"]=>
  string(1) "0"
  ["SS_IS_IDENTITY"]=>
  string(1) "0"
  ["SS_UDT_CATALOG_NAME"]=>
  NULL
  ["SS_UDT_SCHEMA_NAME"]=>
  NULL
  ["SS_UDT_ASSEMBLY_TYPE_NAME"]=>
  NULL
  ["SS_XML_SCHEMACOLLECTION_CATALOG_NAME"]=>
  NULL
  ["SS_XML_SCHEMACOLLECTION_SCHEMA_NAME"]=>
  NULL
  ["SS_XML_SCHEMACOLLECTION_NAME"]=>
  NULL
  ["SS_DATA_TYPE"]=>
  string(2) "39"
}
