--TEST--
Bug #27303 (OCIBindByName binds numeric PHP values as characters)
--SKIPIF--
<?php
if (!extension_loaded('oci8')) die ("skip no oci8 extension");
require(__DIR__."/connect.inc");
// The bind buffer size edge cases seem to change each DB version.
preg_match('/.*Release ([[:digit:]]+)\.([[:digit:]]+)\.([[:digit:]]+)\.([[:digit:]]+)\.([[:digit:]]+)*/', oci_server_version($c), $matches);
if (!(isset($matches[0]) && $matches[1] < 12)) {
    die("skip expected output only valid when using pre-Oracle 12c database");
}
?>
--FILE--
<?php

require(__DIR__.'/connect.inc');

$stmtarray = array(
    "drop sequence myseq",
    "drop table mytab",
    "create sequence myseq",
    "create table mytab (mydata varchar2(20), seqcol number)"
);

oci8_test_sql_execute($c, $stmtarray);

define('MYLIMIT', 200);

$stmt = "insert into mytab (mydata, seqcol) values ('Some data', myseq.nextval) returning seqcol into :mybv";

$stid = oci_parse($c, $stmt);
if (!$stid) { echo "Parse error"; die; }

$r = oci_bind_by_name($stid, ':MYBV', $mybv);
if (!$r) { echo "Bind error"; die; }

for ($i = 1; $i < MYLIMIT; $i++) {
    $r = oci_execute($stid, OCI_DEFAULT);
    if (!$r) { echo "Execute error"; die; }
    var_dump($mybv);
}

oci_commit($c);

$stmtarray = array(
    "drop sequence myseq",
    "drop table mytab"
);

oci8_test_sql_execute($c, $stmtarray);

echo "Done\n";
?>
--EXPECT--
string(1) "1"
string(1) "2"
string(1) "3"
string(1) "4"
string(1) "5"
string(1) "6"
string(1) "7"
string(1) "8"
string(1) "9"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "2"
string(1) "2"
string(1) "2"
string(1) "2"
string(1) "2"
string(1) "2"
string(1) "2"
string(1) "2"
string(1) "2"
string(1) "2"
string(1) "3"
string(1) "3"
string(1) "3"
string(1) "3"
string(1) "3"
string(1) "3"
string(1) "3"
string(1) "3"
string(1) "3"
string(1) "3"
string(1) "4"
string(1) "4"
string(1) "4"
string(1) "4"
string(1) "4"
string(1) "4"
string(1) "4"
string(1) "4"
string(1) "4"
string(1) "4"
string(1) "5"
string(1) "5"
string(1) "5"
string(1) "5"
string(1) "5"
string(1) "5"
string(1) "5"
string(1) "5"
string(1) "5"
string(1) "5"
string(1) "6"
string(1) "6"
string(1) "6"
string(1) "6"
string(1) "6"
string(1) "6"
string(1) "6"
string(1) "6"
string(1) "6"
string(1) "6"
string(1) "7"
string(1) "7"
string(1) "7"
string(1) "7"
string(1) "7"
string(1) "7"
string(1) "7"
string(1) "7"
string(1) "7"
string(1) "7"
string(1) "8"
string(1) "8"
string(1) "8"
string(1) "8"
string(1) "8"
string(1) "8"
string(1) "8"
string(1) "8"
string(1) "8"
string(1) "8"
string(1) "9"
string(1) "9"
string(1) "9"
string(1) "9"
string(1) "9"
string(1) "9"
string(1) "9"
string(1) "9"
string(1) "9"
string(1) "9"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
Done
