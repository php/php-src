--TEST--
odbc_data_source(): Basic test
--EXTENSIONS--
odbc
--SKIPIF--
<?php
include 'skipif.inc';
if (odbc_data_source($conn, SQL_FETCH_FIRST) === NULL) {
    die("skip no data sources defined on this system");
}
?>
--FILE--
<?php

include 'config.inc';

$conn = odbc_connect($dsn, $user, $pass);

try {
    var_dump(odbc_data_source($conn, NULL));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
var_dump(odbc_data_source($conn, SQL_FETCH_FIRST));

?>
--EXPECTF--
odbc_data_source(): Argument #2 ($fetch_type) must be either SQL_FETCH_FIRST or SQL_FETCH_NEXT
array(%d) {
%a
}
