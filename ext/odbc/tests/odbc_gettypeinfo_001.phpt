--TEST--
odbc_gettypeinfo(): Getting info about data types
--EXTENSIONS--
odbc
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php

include 'config.inc';

$conn = odbc_connect($dsn, $user, $pass);

$res = odbc_gettypeinfo($conn, 0);
var_dump(odbc_fetch_array($res));
odbc_free_result($res);

$res = odbc_gettypeinfo($conn, 1);
var_dump(odbc_fetch_array($res));
odbc_free_result($res);

$res = odbc_gettypeinfo($conn, 2);
var_dump(odbc_fetch_array($res));
odbc_free_result($res);

odbc_close($conn);
?>
--EXPECTF--
array(20) {
  ["TYPE_NAME"]=>
  string(14) "datetimeoffset"
  ["DATA_TYPE"]=>
  string(4) "-155"
  ["COLUMN_SIZE"]=>
  string(2) "34"
  ["LITERAL_PREFIX"]=>
  string(1) "'"
  ["LITERAL_SUFFIX"]=>
  string(1) "'"
  ["CREATE_PARAMS"]=>
  string(5) "scale"
  ["NULLABLE"]=>
  string(1) "1"
  ["CASE_SENSITIVE"]=>
  string(1) "0"
  ["SEARCHABLE"]=>
  string(1) "3"
  ["UNSIGNED_ATTRIBUTE"]=>
  NULL
  ["FIXED_PREC_SCALE"]=>
  string(1) "0"
  ["AUTO_UNIQUE_VALUE"]=>
  NULL
  ["LOCAL_TYPE_NAME"]=>
  string(14) "datetimeoffset"
  ["MINIMUM_SCALE"]=>
  string(1) "0"
  ["MAXIMUM_SCALE"]=>
  string(1) "7"
  ["SQL_DATA_TYPE"]=>
  string(4) "-155"
  ["SQL_DATETIME_SUB"]=>
  string(1) "0"
  ["NUM_PREC_RADIX"]=>
  NULL
  ["INTERVAL_PRECISION"]=>
  NULL
  ["USERTYPE"]=>
  string(1) "0"
}
array(20) {
  ["TYPE_NAME"]=>
  string(4) "char"
  ["DATA_TYPE"]=>
  string(1) "1"
  ["COLUMN_SIZE"]=>
  string(4) "8000"
  ["LITERAL_PREFIX"]=>
  string(1) "'"
  ["LITERAL_SUFFIX"]=>
  string(1) "'"
  ["CREATE_PARAMS"]=>
  string(6) "length"
  ["NULLABLE"]=>
  string(1) "1"
  ["CASE_SENSITIVE"]=>
  string(1) "0"
  ["SEARCHABLE"]=>
  string(1) "3"
  ["UNSIGNED_ATTRIBUTE"]=>
  NULL
  ["FIXED_PREC_SCALE"]=>
  string(1) "0"
  ["AUTO_UNIQUE_VALUE"]=>
  NULL
  ["LOCAL_TYPE_NAME"]=>
  string(4) "char"
  ["MINIMUM_SCALE"]=>
  NULL
  ["MAXIMUM_SCALE"]=>
  NULL
  ["SQL_DATA_TYPE"]=>
  string(1) "1"
  ["SQL_DATETIME_SUB"]=>
  NULL
  ["NUM_PREC_RADIX"]=>
  NULL
  ["INTERVAL_PRECISION"]=>
  NULL
  ["USERTYPE"]=>
  string(1) "1"
}
array(20) {
  ["TYPE_NAME"]=>
  string(7) "numeric"
  ["DATA_TYPE"]=>
  string(1) "2"
  ["COLUMN_SIZE"]=>
  string(2) "38"
  ["LITERAL_PREFIX"]=>
  NULL
  ["LITERAL_SUFFIX"]=>
  NULL
  ["CREATE_PARAMS"]=>
  string(15) "precision,scale"
  ["NULLABLE"]=>
  string(1) "1"
  ["CASE_SENSITIVE"]=>
  string(1) "0"
  ["SEARCHABLE"]=>
  string(1) "2"
  ["UNSIGNED_ATTRIBUTE"]=>
  string(1) "0"
  ["FIXED_PREC_SCALE"]=>
  string(1) "0"
  ["AUTO_UNIQUE_VALUE"]=>
  string(1) "0"
  ["LOCAL_TYPE_NAME"]=>
  string(7) "numeric"
  ["MINIMUM_SCALE"]=>
  string(1) "0"
  ["MAXIMUM_SCALE"]=>
  string(2) "38"
  ["SQL_DATA_TYPE"]=>
  string(1) "2"
  ["SQL_DATETIME_SUB"]=>
  NULL
  ["NUM_PREC_RADIX"]=>
  string(2) "10"
  ["INTERVAL_PRECISION"]=>
  NULL
  ["USERTYPE"]=>
  string(2) "10"
}
